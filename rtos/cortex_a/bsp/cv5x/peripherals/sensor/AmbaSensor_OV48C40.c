/**
 *  @file AmbaSensor_OV48C40.c
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
 *  @details Control APIs of OV OV48C40 CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaVIN.h"

#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_OV48C40.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "bsp.h"
#include "AmbaGPIO_Def.h"

//#define AE_DEBUG
//#define GAIN_DEBUG
//#define MASK_SHUTTER_DGAIN  0
#define GROUP_WRITE_SIZE 96U

static UINT32 OV48C40I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
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
/*-----------------------------------------------------------------------------------------------*\
 * OV48C40 sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static OV48C40_CTRL_s OV48C40Ctrl = {0};
//static UINT32 OV48C40_CitLshift = 0;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_PrepareModeInfo
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
static void OV48C40_PrepareModeInfo(const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal                                       = ERR_NONE;
    UINT32 ModeID                                       = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s      *pInputInfo     = &OV48C40InputInfoNormalReadout[ModeID];
    const AMBA_SENSOR_OUTPUT_INFO_s     *pOutputInfo    = &OV48C40OutputInfo[ModeID];
    const OV48C40_FRAME_TIMING_s    *pFrameTiming       = &OV48C40ModeInfoList[ModeID].FrameTiming;

    pModeInfo->LineLengthPck            = pFrameTiming->Linelengthpck;
    pModeInfo->FrameLengthLines         = pFrameTiming->FrameLengthLines;
    pModeInfo->NumExposureStepPerFrame  = pFrameTiming->FrameLengthLines;

    pModeInfo->InputClk = pFrameTiming->InputClk;

    pModeInfo->RowTime = (FLOAT)((DOUBLE)pFrameTiming->Linelengthpck / (DOUBLE) OV48C40ModeInfoList[ModeID].FrameTiming.PixelRate);

    if ((AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s)) != ERR_NONE) ||
        (AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTiming->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) != ERR_NONE)) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    /* HDR information */
    if(OV48C40ModeInfoList[ModeID].HdrType == AMBA_SENSOR_HDR_NONE) {
        pModeInfo->MinFrameRate.TimeScale = (UINT32)(pModeInfo->MinFrameRate.TimeScale/8U);
        pModeInfo->HdrInfo.HdrType = AMBA_SENSOR_HDR_NONE;
        pModeInfo->HdrInfo.ActiveChannels = 0;
    } else if (OV48C40ModeInfoList[ModeID].HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
//        pModeInfo->HdrInfo.NotSupportIndividualGain = 1;  // individual D gain?  need to be fixed!!!!!
        AmbaPrint_PrintUInt5("ActiveChannels:%d, HdrType:%d", OV48C40HdrInfo[ModeID].ActiveChannels, OV48C40HdrInfo[ModeID].HdrType, 0U, 0U, 0U);
        if (AmbaWrap_memcpy(&pModeInfo->HdrInfo, &OV48C40HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    } else if (OV48C40ModeInfoList[ModeID].HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
//        pModeInfo->HdrInfo.NotSupportIndividualGain = 1;  // individual D gain?  need to be fixed!!!!!
        AmbaPrint_PrintUInt5("ActiveChannels:%d, HdrType:%d", OV48C40HdrInfo[ModeID].ActiveChannels, OV48C40HdrInfo[ModeID].HdrType, 0U, 0U, 0U);
        if (AmbaWrap_memcpy(&pModeInfo->HdrInfo, &OV48C40HdrInfo[ModeID], sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    } else {
        // MisraC
    }

    if (RetVal != ERR_NONE) {
        /* misra */
    }

    return;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_ConfigVin
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
static void OV48C40_ConfigVin(const UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s OV48C40VinConfig = {
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
        .VirtChanHDRConfig = {
                .VirtChanNum = 2,
                .VCPattern1stExp = 0,
                .VCPattern2ndExp = 1,
                .VCPattern3rdExp = 2,
                .VCPattern4thExp = 3,
                .Offset2ndExp = 0,
                .Offset3rdExp = 0,
                .Offset4thExp = 0,
        },
    };
    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    AMBA_VIN_MIPI_CONFIG_s *pVinCfg = &OV48C40VinConfig;
    UINT32 RetVal = ERR_NONE;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->Config.BayerPattern = pImgInfo->BayerPattern;
    pVinCfg->Config.NumDataBits = pImgInfo->NumDataBits;

    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels  = pImgInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines   = pImgInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;

    if (pImgInfo->NumDataBits == 12U) {
        pVinCfg->DataType = 0x2C;
    } else if (pImgInfo->NumDataBits == 14U) {
        pVinCfg->DataType = 0x2D;
    } else {
        pVinCfg->DataType = 0x2B;
    }

    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
        pVinCfg->VirtChanHDREnable = 1;
        pVinCfg->VirtChanHDRConfig.VirtChanNum = pModeInfo->HdrInfo.ActiveChannels;
        pVinCfg->VirtChanHDRConfig.VCPattern1stExp = 0U;
        pVinCfg->VirtChanHDRConfig.VCPattern2ndExp = 1U;
        pVinCfg->VirtChanHDRConfig.Offset2ndExp = pModeInfo->HdrInfo.ChannelInfo[1].EffectiveArea.StartY+16U;
    } else {
        pVinCfg->VirtChanHDREnable = 0;
    }

    if (AmbaWrap_memcpy(&pVinCfg->Config.FrameRate, &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) == ERR_NONE) {
        RetVal = AmbaVIN_MipiCphyConfig(VinID, pVinCfg);
    }
    if (RetVal != ERR_NONE) {
        /* misra */
    }

    return;
}

static UINT32 OV48C40_RegRead(UINT32 VinID, UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = OV48C40_I2C_SLAVE_ADDRESS;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr >> 8U);
    TxData[1] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.SlaveAddr = ((UINT32)OV48C40_I2C_SLAVE_ADDRESS | (UINT32)1U);
    I2cRxConfig.DataSize  = Size;
    I2cRxConfig.pDataBuf  = pRxData;
    RetVal = AmbaI2C_MasterReadAfterWrite(OV48C40I2cChannel[VinID], AMBA_I2C_SPEED_FAST, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);
    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_PrintStr5("[OV48C40] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

static UINT32 OV48C40_RegWrite(UINT32 VinID, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[128U];
    UINT32 TxSize, i;

    if (Size > 126U) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.SlaveAddr = OV48C40_I2C_SLAVE_ADDRESS;
        I2cConfig.DataSize  = (2U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pTxData[i];
        }

        RetVal = AmbaI2C_MasterWrite(OV48C40I2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                                     &I2cConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_PrintStr5("[OV48C40] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }
    }

    //AmbaPrint_PrintUInt5("[OV48C40][IO] Addr = 0x%04x, Data = 0x%02x", Addr, pTxData[0], 0, 0, 0);
    //SENSOR_DBG_IO("[OV48C40][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_RegisterRead
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
static UINT32 OV48C40_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *Data)
{
    UINT32 RetVal;
    UINT8 RxData;

    RetVal = OV48C40_RegRead(pChan->VinID, Addr, &RxData, 1U);

    if (RetVal == SENSOR_ERR_NONE) {
        *Data = RxData;
    }

    return SENSOR_ERR_NONE;
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
static UINT32 OV48C40_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal;
    UINT8 TxData = (UINT8) Data;

    RetVal = OV48C40_RegWrite(pChan->VinID, Addr, &TxData, 1);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_HardwareReset
 *
 *  @Description:: Reset OV48C40 Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static inline void OV48C40_HardwareReset(UINT32 VinId)
{
    if (AmbaUserGPIO_SensorResetCtrl(VinId, AMBA_GPIO_LEVEL_LOW) == ERR_NONE) {
        if (AmbaKAL_TaskSleep(2) == ERR_NONE) {               /* XCLR Low level pulse width >= 100ns */
            if (AmbaUserGPIO_SensorResetCtrl(VinId, AMBA_GPIO_LEVEL_HIGH) == ERR_NONE) {
                if(AmbaKAL_TaskSleep(2) == ERR_NONE) {
                    /* MisraC */
                }
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV48C40_SetStandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 RegData = 0x0;
    UINT32 RetVal;
    RetVal = OV48C40_RegWrite(pChan->VinID, OV48C40_MODE_SEL_REG, &RegData, 1);

    if (RetVal != ERR_NONE) {
        /* misra */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV48C40_SetStandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT8 RegData = 0x1;
    UINT32 RetVal;

    RetVal = OV48C40_RegWrite(pChan->VinID, OV48C40_MODE_SEL_REG, &RegData, 1);

    if (RetVal != ERR_NONE) {
        /* misra */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void OV48C40_PreCalculateModeInfo(void)
{
    UINT8 i;

    for (i = 0U; i < OV48C40_NUM_MODE; i++) {
        OV48C40ModeInfoList[i].RowTime = (FLOAT)((DOUBLE)OV48C40ModeInfoList[i].FrameTiming.Linelengthpck /
                                         ((DOUBLE)OV48C40OutputInfo[i].DataRate
                                          * (DOUBLE)OV48C40OutputInfo[i].NumDataLanes
                                          / (DOUBLE)OV48C40OutputInfo[i].NumDataBits));

        //AmbaPrint("row time: %f", OV48C40ModeInfoList[i].RowTime);
    }
}

static UINT32 OV48C40_GroupMid1(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SensorMode)
{
    UINT32 i = 0U, j = 0U;
    UINT32 RetVal = ERR_NONE;
    UINT8 TxData[GROUP_WRITE_SIZE];

    for (i = 0U; i < OV48C40_NUM_READOUT_MODE_REG_MID1; i+=GROUP_WRITE_SIZE) {
          for (j =0U; j<GROUP_WRITE_SIZE; j++) {
               TxData[j] = OV48C40ModeRegTableMid1[i+j].Data[SensorMode];
          }
          RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40ModeRegTableMid1[i].Addr, TxData, GROUP_WRITE_SIZE);
    }
    return RetVal;
}

static UINT32 OV48C40_GroupMid2(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SensorMode)
{
    UINT32 i = 0U, j = 0U;
    UINT32 RetVal = ERR_NONE;
    UINT8 TxData[GROUP_WRITE_SIZE];

    for (i = 0U; i < OV48C40_NUM_READOUT_MODE_REG_MID2; i+=GROUP_WRITE_SIZE) {
          for (j =0U; j<GROUP_WRITE_SIZE; j++) {
               TxData[j] = OV48C40ModeRegTableMid2[i+j].Data[SensorMode];
          }
          RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40ModeRegTableMid2[i].Addr, TxData, GROUP_WRITE_SIZE);
    }

    return RetVal;
}

static UINT32 OV48C40_DisablePD(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 i = 0U, j = 0U;
    UINT32 RetVal = ERR_NONE;
    UINT8 TxData[GROUP_WRITE_SIZE];

    for (i = 0U; i < OV48C40_NUM_DISABLE_PD_REGS; i+=GROUP_WRITE_SIZE) {
          for (j =0U; j<GROUP_WRITE_SIZE; j++) {
               TxData[j] = OV48C40_DISABLE_PD_REG_TABLE[i+j].Data;
          }
          RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40_DISABLE_PD_REG_TABLE[i].Addr, TxData, GROUP_WRITE_SIZE);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_ChangeReadoutMode
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
static UINT32 OV48C40_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SensorMode)
{
    UINT32 i = 0;
    UINT32 RetVal = ERR_NONE;
    UINT32 DcgVsMode = 0U;
    UINT32 PostMappingIndex = OV48C40_NUM_MODE;

    if ((SensorMode == OV48C40_4032_3024_27P_DCG_VS_HDR) || 
        (SensorMode == OV48C40_4032_3024_25P_DCG_VS_HDR) ||
        (SensorMode == OV48C40_4032_3024_24P_DCG_VS_HDR) ||
        (SensorMode == OV48C40_2016_1512_30P_DCG_VS_HDR) ||
        (SensorMode == OV48C40_2016_1512_25P_DCG_VS_HDR)) {
        DcgVsMode = 1U;
    }

    // Configure general setting
    if (DcgVsMode == 0U) {
        for (i = 0; i < OV48C40_NUM_READOUT_MODE_REG_TOP; i++) {
            RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40ModeRegTableTop[i].Addr, &OV48C40ModeRegTableTop[i].Data[SensorMode], 1U);
       }

       RetVal |= OV48C40_GroupMid1(pChan, SensorMode);
       RetVal |= OV48C40_GroupMid2(pChan, SensorMode);

       for (i = 0; i < OV48C40_NUM_READOUT_MODE_REG_BOTTOM; i++) {
            RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40ModeRegTableBottom[i].Addr, &OV48C40ModeRegTableBottom[i].Data[SensorMode], 1U);
       }
    } else {
        for (i = 0; i < OV48C40_NUM_READOUT_MODE_REG_TOP; i++) {
            RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40ModeRegTableTop[i].Addr, &OV48C40ModeRegTableTop[i].Data[SensorMode], 1U);
            if (i==1U) {
                UINT32 j =0;
                for (j = 0; j < OV48C40_NUM_DCG_VS_REGS; j++) {
                    RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40_DCG_VS_REG_TABLE[j].Addr, &OV48C40_DCG_VS_REG_TABLE[j].Data, 1U);
                }
            }
       }

       RetVal |= OV48C40_GroupMid1(pChan, SensorMode);
       RetVal |= OV48C40_GroupMid2(pChan, SensorMode);

       for (i = 0; i < OV48C40_NUM_READOUT_MODE_REG_BOTTOM; i++) {
            RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40ModeRegTableBottom[i].Addr, &OV48C40ModeRegTableBottom[i].Data[SensorMode], 1U);
       }
    }

    for ( i = 0; i < OV48C40_NUM_POST_REG_MODES; i++) {
         if (SensorMode == PostRegSensorModeMapping[i]) {
             PostMappingIndex = i;
             break;
         }
    }

    if((SensorMode == OV48C40_7680_4320_24P) ||
       (SensorMode == OV48C40_8064_6048_15P) ||
       (SensorMode == OV48C40_6352_5512_15P) ||
       (SensorMode == OV48C40_6352_3834_30P) ||
       (SensorMode == OV48C40_3680_2756_30P) ||
       (SensorMode == OV48C40_3680_2068_30P)) {
        RetVal |= OV48C40_DisablePD(pChan);

        if(PostMappingIndex < OV48C40_NUM_MODE) {
           for (i = 0; i < OV48C40_NUM_POST_REGS; i++) {
                RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40ModePostRegTable[i].Addr, &OV48C40ModePostRegTable[i].Data[PostMappingIndex], 1U);
           }
        }

    } else if((SensorMode == OV48C40_4032_3024_60P) ||
              (SensorMode == OV48C40_4032_3024_50P) ||
              (SensorMode == OV48C40_4032_3024_48P) ||
              (SensorMode == OV48C40_4032_3024_30P) ||
              (SensorMode == OV48C40_4032_3024_25P) ||
              (SensorMode == OV48C40_4032_3024_24P) ||
              (SensorMode == OV48C40_4032_3024_30P_DCG_HDR) ||
              (SensorMode == OV48C40_4032_3024_25P_DCG_HDR) ||
              (SensorMode == OV48C40_4032_3024_24P_DCG_HDR)) {
        if ( PostMappingIndex < OV48C40_NUM_MODE) {
            for (i = 0; i < OV48C40_NUM_POST_REGS; i++) {
                 RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40ModePostRegTable[i].Addr, &OV48C40ModePostRegTable[i].Data[PostMappingIndex], 1U);
            }

            for (i = 0; i < OV48C40_NUM_4032_3024_END_REGS; i++) {
                RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40_4032_3024_END_REG_TABLE[i].Addr, &OV48C40_4032_3024_END_REG_TABLE[i].Data, 1U);
            }
        } else {
            AmbaPrint_PrintUInt5("Please Check Sensor Mode %d Mapping index %d", SensorMode, PostMappingIndex, 0U, 0U, 0U);
        }
    } else {
        if(PostMappingIndex < OV48C40_NUM_MODE) {
           for (i = 0; i < OV48C40_NUM_POST_REGS; i++) {
                RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40ModePostRegTable[i].Addr, &OV48C40ModePostRegTable[i].Data[PostMappingIndex], 1U);
           }
        }
    }

    RetVal |= AmbaWrap_memset(OV48C40Ctrl.CurrentAgc, 0, sizeof(OV48C40Ctrl.CurrentAgc));
    RetVal |= AmbaWrap_memset(OV48C40Ctrl.CurrentDgc, 0, sizeof(OV48C40Ctrl.CurrentDgc));
    RetVal |= AmbaWrap_memset(OV48C40Ctrl.CurrentShrCtrl, 0, sizeof(OV48C40Ctrl.CurrentShrCtrl));

    if (RetVal != ERR_NONE) {
        // misrac
    }
    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_Init
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
static UINT32 OV48C40_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const OV48C40_MODE_INFO_s *pSensorInfo = &OV48C40ModeInfoList[0];

    if(pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pSensorInfo->FrameTiming.InputClk);
        OV48C40_PreCalculateModeInfo();
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_Enable
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
static UINT32 OV48C40_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    AmbaMisra_TouchUnused(&pChan);

    /* Config mipi phy */
    //AmbaDSP_VinPhySetMIPI(pChan->VinID);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_Disable
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
static UINT32 OV48C40_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    /* Sensor is at Standby mode */
    OV48C40_SetStandbyOn(pChan);

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_GetModeInfo
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
static UINT32 OV48C40_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    AmbaMisra_TouchUnused(&pChan);
    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = OV48C40Ctrl.Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }

        if (Config.ModeID >= OV48C40_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            OV48C40_PrepareModeInfo(&Config, pModeInfo);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_GetDeviceInfo
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
static UINT32 OV48C40_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    if (pDeviceInfo == NULL) {
        RetVal = SENSOR_ERR_ARG;
    }

    AmbaMisra_TouchUnused(&pChan);

    if (RetVal == SENSOR_ERR_NONE) {
        if (AmbaWrap_memcpy(pDeviceInfo, &OV48C40DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_GetHdrInfo
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
static UINT32 OV48C40_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pShutterCtrl);

    if (AmbaWrap_memcpy(pHdrInfo, &OV48C40Ctrl.Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s)) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_GetCurrentGainFactor
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
static UINT32 OV48C40_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    FLOAT AnalogGainFactor = 0.0f;
    FLOAT DigitalGainFactor = 0.0f;
    UINT32 i = 0U;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pGainFactor == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (OV48C40Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            AnalogGainFactor = (FLOAT)OV48C40Ctrl.CurrentAgc[0]/256.0f;
            DigitalGainFactor = (FLOAT)OV48C40Ctrl.CurrentDgc[0] / 1024.0f;
            *pGainFactor = AnalogGainFactor * DigitalGainFactor;
        } else if (OV48C40Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < (OV48C40Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                AnalogGainFactor = (FLOAT) OV48C40Ctrl.CurrentAgc[i]/ 256.0f;
                DigitalGainFactor = (FLOAT)OV48C40Ctrl.CurrentDgc[i] / 1024.0f;
                pGainFactor[i] = AnalogGainFactor * DigitalGainFactor;
            }
        } else if (OV48C40Ctrl.Status.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
            for (i = 0U; i < (OV48C40Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i ++) {
                AnalogGainFactor = (FLOAT) OV48C40Ctrl.CurrentAgc[i]/ 256.0f;
                DigitalGainFactor = (FLOAT)OV48C40Ctrl.CurrentDgc[i] / 1024.0f;
                pGainFactor[i] = AnalogGainFactor * DigitalGainFactor;
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pGainFactor);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_GetCurrentShutterSpeed
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
static UINT32 OV48C40_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OV48C40Ctrl.Status.ModeInfo;
//    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
//    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 i;

    AmbaMisra_TouchUnused(&pChan);

    if (pExposureTime == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pExposureTime[0] = (pModeInfo->RowTime * (FLOAT)OV48C40Ctrl.CurrentShrCtrl[0]);
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] =  (pModeInfo->RowTime * (FLOAT)OV48C40Ctrl.CurrentShrCtrl[i]);
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                pExposureTime[i] =  (pModeInfo->RowTime * (FLOAT)OV48C40Ctrl.CurrentShrCtrl[i]);
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_GetStatus
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
static UINT32 OV48C40_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AmbaMisra_TouchUnused(&pChan);

    if (pStatus == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pStatus, &OV48C40Ctrl.Status, sizeof(AMBA_SENSOR_STATUS_INFO_s)) != ERR_NONE) {
            RetVal = SENSOR_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

static void ConvertGainFactor(FLOAT DesiredFactor, FLOAT* pActualFactor, UINT32* pAnalogGainCtrl, UINT32* pDigitalGainCtrl)
{
    if (DesiredFactor < 1.0 ) {
        FLOAT AnalogGainFactor;
        *pAnalogGainCtrl = 0x100;
        *pDigitalGainCtrl = 0x400;
        AnalogGainFactor = 1.0f;
        *pActualFactor =  AnalogGainFactor;
    } else {
        FLOAT AnalogGainFactor;
        FLOAT DigitalGainFactor;
        FLOAT MaxAnalogGainFactor = OV48C40DeviceInfo.MaxAnalogGainFactor;
        UINT16 AnalogGainCtrlFactor = 0U;
        DOUBLE DigitalGainCtrlInDb;
        DOUBLE AnalogGainCtrlInDb;

        AnalogGainCtrlFactor = 0xf80;
        MaxAnalogGainFactor = 15.5f;

        DigitalGainFactor = DesiredFactor / MaxAnalogGainFactor;

        if (DigitalGainFactor <= 1.0) {
            *pDigitalGainCtrl = 0x400U;
            DigitalGainFactor = 1.0f;
            AnalogGainFactor = DesiredFactor;
        } else {
            if (DigitalGainFactor > OV48C40DeviceInfo.MaxDigitalGainFactor) {
                DigitalGainFactor = OV48C40DeviceInfo.MaxDigitalGainFactor;
            }

            if (AmbaWrap_floor((DigitalGainFactor *1024.0), &DigitalGainCtrlInDb) == ERR_NONE) {

                *pDigitalGainCtrl = (UINT32) DigitalGainCtrlInDb;

                DigitalGainFactor = (FLOAT)*pDigitalGainCtrl/ 1024.0f;

                AnalogGainFactor = DesiredFactor /DigitalGainFactor;
            } else {
                AnalogGainFactor = 1.0f;
            }
        }

        if (AnalogGainFactor > 15.5f) {
            AnalogGainFactor = 15.5f;
        } else if (AnalogGainFactor <= 1.0f) {
            AnalogGainFactor = 1.0f;
        } else if (AnalogGainFactor <= 2.0f) {
            AnalogGainFactor = ((INT32)(AnalogGainFactor * 16.0f))/16.0f;
        } else if (AnalogGainFactor <= 4.0f) {
            AnalogGainFactor = ((INT32)(AnalogGainFactor * 8.0f))/8.0f;
        } else if (AnalogGainFactor <= 8.0f) {
            AnalogGainFactor = ((INT32)(AnalogGainFactor * 4.0f))/4.0f;
        } else {
            AnalogGainFactor = ((INT32)(AnalogGainFactor * 2.0f))/2.0f;
        }

        if (AmbaWrap_floor((AnalogGainFactor* 256.0), &AnalogGainCtrlInDb) == ERR_NONE) {
            *pAnalogGainCtrl = (UINT16)AnalogGainCtrlInDb;
            if (*pAnalogGainCtrl > AnalogGainCtrlFactor) {
                *pAnalogGainCtrl = AnalogGainCtrlFactor;
            }
            if (*pAnalogGainCtrl < (UINT32) 0x100) {
                *pAnalogGainCtrl = 0x100;
            }
        } else {
            *pAnalogGainCtrl = 0x100;
        }

        AnalogGainFactor = (FLOAT)(((FLOAT)(*pAnalogGainCtrl))/256.0f);
        *pActualFactor = AnalogGainFactor * DigitalGainFactor;
    }

#if defined(GAIN_DEBUG)
    //    Chan.Bits.VinID, Chan.Bits.HdrID, DesiredFactor, *pAnalogGainCtrl, *pDigitalGainCtrl, *pActualFactor);
    AmbaPrint_PrintUInt5("[OV48C][GAINCONV] DesiredFactor:%ld, AnalogGainCtrl:%ld, DigitalGainCtrl:%ld, ActualFactor:%ld", DesiredFactor,  *pAnalogGainCtrl, *pDigitalGainCtrl, *pActualFactor, 0);
#endif

}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_ConvertGainFactor
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
static UINT32 OV48C40_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s* pModeInfo = &OV48C40Ctrl.Status.ModeInfo;
    UINT8 i;

    if ((pChan == NULL) || (pDesiredFactor == NULL) || (pActualFactor == NULL) || (pGainCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertGainFactor(pDesiredFactor->Gain[0], &(pActualFactor->Gain[0]), &(pGainCtrl->AnalogGain[0]), &(pGainCtrl->DigitalGain[0]));
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i++) {
                ConvertGainFactor(pDesiredFactor->Gain[i], &(pActualFactor->Gain[i]), &(pGainCtrl->AnalogGain[i]), &(pGainCtrl->DigitalGain[i]));
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
            for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i++) {
                ConvertGainFactor(pDesiredFactor->Gain[i], &(pActualFactor->Gain[i]), &(pGainCtrl->AnalogGain[i]), &(pGainCtrl->DigitalGain[i]));
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pDesiredFactor);
    AmbaMisra_TouchUnused(&pActualFactor);
    AmbaMisra_TouchUnused(&pGainCtrl);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_ConvertShutterSpeed
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
static UINT32 ConvertShutterSpeed(const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl, UINT32 HdrChannel)
{
    UINT32 RetVal                = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OV48C40Ctrl.Status.ModeInfo;
    UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 MaxExposureLineNumber = 0U;
    UINT32 MinExposureLineNumber = 0U;
    UINT32 SensorMode = OV48C40Ctrl.Status.ModeInfo.Config.ModeID;
    DOUBLE                      CtrlInDb;

    if (AmbaWrap_floor((DOUBLE) *pDesiredExposureTime/pModeInfo->RowTime, &CtrlInDb) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    *pShutterCtrl = (UINT32) CtrlInDb;

    if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
        switch (SensorMode) {
            case OV48C40_7680_4320_24P:
            case OV48C40_6352_3834_30P:
            case OV48C40_8064_6048_15P:
            case OV48C40_6352_5512_15P:
            case OV48C40_3680_2756_30P:
            case OV48C40_3680_2068_30P:
                MaxExposureLineNumber = NumExposureStepPerFrame- 32U;
                MinExposureLineNumber = 16U;
                break;
            case OV48C40_1920_1080_30P:
            case OV48C40_1280_720_480P:
            case OV48C40_2016_1512_240P:
            case OV48C40_2016_1512_120P:
            case OV48C40_2016_1512_60P:
            case OV48C40_2016_1512_30P:
            case OV48C40_2016_1512_25P:
                MaxExposureLineNumber = NumExposureStepPerFrame -8U;
                MinExposureLineNumber = 8U;
                break;
            default:
                MaxExposureLineNumber = NumExposureStepPerFrame -16U;
                MinExposureLineNumber = 8U;
                break;
        }
    } else if(pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
        MaxExposureLineNumber = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        MinExposureLineNumber = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
    } else if(pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
        MaxExposureLineNumber = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        MinExposureLineNumber = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
    } else {
        //MisraC
    }

    if (*pShutterCtrl > MaxExposureLineNumber) {
        *pShutterCtrl = MaxExposureLineNumber;
    }

    if (*pShutterCtrl < MinExposureLineNumber) {
        *pShutterCtrl = MinExposureLineNumber;
    }

    *pActualExposureTime = (FLOAT)*pShutterCtrl * (pModeInfo->RowTime * (FLOAT)pModeInfo->FrameLengthLines / (FLOAT)NumExposureStepPerFrame);

//     AmbaPrint_PrintUInt5("[OV48C][SHRCONV]DCG Shutter: D=%d A=%d SC=%d RowT=%d", (UINT32)(*pDesiredExposureTime*1000), (UINT32)(*pActualExposureTime*1000) ,*pShutterCtrl, pModeInfo->RowTime, 0);

    return RetVal;
}

static UINT32 OV48C40_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s* pModeInfo = &OV48C40Ctrl.Status.ModeInfo;
    UINT32 i = 0;

    if ((pChan == NULL) || (pDesiredExposureTime == NULL) || (pActualExposureTime == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal |= ConvertShutterSpeed(pDesiredExposureTime, pActualExposureTime, pShutterCtrl, 0U);
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                RetVal |= ConvertShutterSpeed(&pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i], i);
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
            for(i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i ++) {
                RetVal |= ConvertShutterSpeed(&pDesiredExposureTime[i], &pActualExposureTime[i], &pShutterCtrl[i], i);
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pDesiredExposureTime);
    AmbaMisra_TouchUnused(&pActualExposureTime);
    AmbaMisra_TouchUnused(&pShutterCtrl);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_SetAnalogGainCtrl
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
static UINT32 OV48C40_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OV48C40Ctrl.Status.ModeInfo;
    UINT32 i;
    UINT8 TxData[2U];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pAnalogGainCtrl[0] >> 8U) & 0x00ffU);
            TxData[1] = (UINT8) (pAnalogGainCtrl[0] & 0x00ffU);
            RetVal |= OV48C40_RegWrite(pChan->VinID, (UINT16) OV48C40_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);

            /* Update current AGC control */
            OV48C40Ctrl.CurrentAgc[0] = pAnalogGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for(i = 0U; i < (OV48C40Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i++) {
                UINT16 Addr = 0U;
                TxData[0] = (UINT8)((pAnalogGainCtrl[i] >> 8U) & 0x00ffU);
                TxData[1] = (UINT8) (pAnalogGainCtrl[i] & 0x00ffU);
                if (i == 0U) {
                    Addr= (UINT16) OV48C40_ANA_GAIN_GLOBAL_MSB_REG;
                } else if (i ==1U) {
                    Addr= (UINT16) OV48C40_ANA_GAIN_MED_MSB_REG;
                } else if (i ==2U) {
                    Addr= (UINT16) OV48C40_ANA_GAIN_SHORT_MSB_REG;
                } else {
                    Addr= (UINT16) OV48C40_ANA_GAIN_GLOBAL_MSB_REG;
                }
                RetVal |= OV48C40_RegWrite(pChan->VinID, Addr, TxData, 2);

                /* Update current AGC control */
                if ( i == 0U) {
                    OV48C40Ctrl.CurrentAgc[0] = pAnalogGainCtrl[i];
                } else if ( i == 1U) {
                    OV48C40Ctrl.CurrentAgc[1] = pAnalogGainCtrl[i];
                } else if ( i == 2U) {
                    OV48C40Ctrl.CurrentAgc[2] = pAnalogGainCtrl[i];
                } else {
                    //misrac
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
            for(i = 0U; i < (OV48C40Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i++) {
                TxData[0] = (UINT8)((pAnalogGainCtrl[i] >> 8U) & 0x00ffU);
                TxData[1] = (UINT8) (pAnalogGainCtrl[i] & 0x00ffU);
                if (i==0U) {
                    RetVal |= OV48C40_RegWrite(pChan->VinID, (UINT16) OV48C40_ANA_GAIN_GLOBAL_MSB_REG, TxData, 2);
                    TxData[0] = (UINT8)(((pAnalogGainCtrl[i]/4U) >> 8U) & 0x00ffU);
                    TxData[1] = (UINT8) ((pAnalogGainCtrl[i]/4U) & 0x00ffU);
                    RetVal |= OV48C40_RegWrite(pChan->VinID, (UINT16) OV48C40_ANA_GAIN_MED_MSB_REG, TxData, 2);
                } else {
                    RetVal |= OV48C40_RegWrite(pChan->VinID, (UINT16) OV48C40_ANA_GAIN_SHORT_MSB_REG, TxData, 2);
                }

                /* Update current AGC control */
                if ( i == 0U) {
                    OV48C40Ctrl.CurrentAgc[0] = pAnalogGainCtrl[i];
                } else if (i == 1U) {
                    OV48C40Ctrl.CurrentAgc[1] = pAnalogGainCtrl[i];
                } else {
                    //misrac
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pAnalogGainCtrl);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_SetDigitalGainCtrl
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
static UINT32 OV48C40_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OV48C40Ctrl.Status.ModeInfo;
    UINT8 TxData[3U];
#if defined(MASK_SHUTTER_DGAIN)
    return RetVal;
#endif
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 10U) & 0x0fU);
            TxData[1] = (UINT8) ((pDigitalGainCtrl[0] >> 2)& 0x00ffU);
            TxData[2] = (UINT8) (pDigitalGainCtrl[0] &0x03U);

            RetVal |= OV48C40_RegWrite(pChan->VinID, (UINT16) OV48C40_DGAIN_GLOBAL_COARSE_REG, TxData, 3);
            OV48C40Ctrl.CurrentDgc[0] = pDigitalGainCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            // OV48C40 one dgain to control internal blending now
            // use long frame as dgain control
            UINT16 Addr = (UINT16) OV48C40_DGAIN_GLOBAL_COARSE_REG;
            TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 10U) & 0x0fU);
            TxData[1] = (UINT8) ((pDigitalGainCtrl[0] >> 2)& 0x00ffU);
            TxData[2] = (UINT8) (pDigitalGainCtrl[0] &0x03U);

            RetVal |= OV48C40_RegWrite(pChan->VinID, Addr, TxData, 3);
            OV48C40Ctrl.CurrentDgc[0] = pDigitalGainCtrl[0];
            OV48C40Ctrl.CurrentDgc[1] = pDigitalGainCtrl[1];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
            // OV48C40 only has one dgain for HDR control
            // use long frame as dgain control
            UINT16 Addr = (UINT16)OV48C40_DGAIN_GLOBAL_COARSE_REG;
            TxData[0] = (UINT8)((pDigitalGainCtrl[0] >> 10U) & 0x0fU);
            TxData[1] = (UINT8) ((pDigitalGainCtrl[0] >> 2)& 0x00ffU);
            TxData[2] = (UINT8) (pDigitalGainCtrl[0] &0x03U);

            RetVal |= OV48C40_RegWrite(pChan->VinID, Addr, TxData, 3);
            OV48C40Ctrl.CurrentDgc[0] = pDigitalGainCtrl[0];
            OV48C40Ctrl.CurrentDgc[1] = pDigitalGainCtrl[1];
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }

    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pDigitalGainCtrl);
    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_SetShutterCtrl
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
static UINT32 OV48C40_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OV48C40Ctrl.Status.ModeInfo;
    UINT32 i;
    UINT8 TxData[3U];

#if defined(AE_DEBUG)
    static UINT32 shutter0 = 0;
    static UINT32 shutter1 = 0;
//    static UINT32 shutter2 = 0;
    AmbaPrint_PrintUInt5("[OV48C][SHRCONV]DCG Shutter debug start pShutterCtrl[0]=%u", *pShutterCtrl, 0, 0, 0, 0);
#endif

#if defined(MASK_SHUTTER_DGAIN)
    return RetVal;
#endif

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        AmbaPrint_PrintUInt5("[OV48C][SHRCONV]DCG Shutter debug NULL", 0, 0, 0, 0, 0);
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            TxData[0] = (UINT8)((pShutterCtrl[0] >> 16U) & 0x00ffU);
            TxData[1] = (UINT8)((pShutterCtrl[0] >>8U) & 0x00ffU);
            TxData[2] = (UINT8) (pShutterCtrl[0] & 0x00ffU);

            RetVal |= OV48C40_RegWrite(pChan->VinID, OV48C40_EXPO_COARSE_HIGH_REG, TxData, 3);

            /* Update current shutter control */
            OV48C40Ctrl.CurrentShrCtrl[0] = pShutterCtrl[0];
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            for(i = 0U; i < (OV48C40Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i++) {
                UINT16 Addr = 0U;
                TxData[0] = (UINT8)((pShutterCtrl[i] >> 16U) & 0x00ffU);
                TxData[1] = (UINT8)((pShutterCtrl[i] >>8U) & 0x00ffU);
                TxData[2] = (UINT8) (pShutterCtrl[i] & 0x00ffU);
                if (i == 0U) {
                    Addr = (UINT16) OV48C40_EXPO_COARSE_HIGH_REG;
                } else if( i == 1U) {
                    Addr = (UINT16) OV48C40_EXPO_MED_HIGH_REG;
                } else if( i == 2U) {
                    Addr = (UINT16) OV48C40_EXPO_SHORT_HIGH_REG;
                } else {
                    Addr = (UINT16) OV48C40_EXPO_COARSE_HIGH_REG;
                }
                RetVal |= OV48C40_RegWrite(pChan->VinID, Addr, TxData, 3);
                /* Update current shutter control */
                if ( i == 0U) {
                    OV48C40Ctrl.CurrentShrCtrl[0] = pShutterCtrl[i];
                } else if ( i == 1U) {
                    OV48C40Ctrl.CurrentShrCtrl[1] = pShutterCtrl[i];
                } else if ( i == 2U) {
                    OV48C40Ctrl.CurrentShrCtrl[2] = pShutterCtrl[i];
                } else {
                    //misrac
                }
            }
        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
            for(i = 0U; i < (OV48C40Ctrl.Status.ModeInfo.HdrInfo.ActiveChannels); i++) {
                UINT16 Addr = 0U;
                TxData[0] = (UINT8)((pShutterCtrl[i] >> 16U) & 0x00ffU);
                TxData[1] = (UINT8)((pShutterCtrl[i] >>8U) & 0x00ffU);
                TxData[2] = (UINT8) (pShutterCtrl[i] & 0x00ffU);
                if (i == 0U) {
                    Addr = (UINT16)OV48C40_EXPO_COARSE_HIGH_REG;
                    RetVal |= OV48C40_RegWrite(pChan->VinID, Addr, TxData, 3);
                    /* Update current shutter control */
                    OV48C40Ctrl.CurrentShrCtrl[i] = pShutterCtrl[i];
                    Addr = (UINT16) OV48C40_EXPO_MED_HIGH_REG;
                    RetVal |= OV48C40_RegWrite(pChan->VinID, Addr, TxData, 3);
                } else if( i == 1U) {
                    Addr = (UINT16) OV48C40_EXPO_SHORT_HIGH_REG;
                    RetVal |= OV48C40_RegWrite(pChan->VinID, Addr, TxData, 3);
                    /* Update current shutter control */
                    OV48C40Ctrl.CurrentShrCtrl[i] = pShutterCtrl[i];
                } else {  //i == 2
                    Addr = (UINT16) OV48C40_EXPO_COARSE_HIGH_REG;
                    RetVal |= OV48C40_RegWrite(pChan->VinID, Addr, TxData, 3);
                    /* Update current shutter control */
                    OV48C40Ctrl.CurrentShrCtrl[2] = pShutterCtrl[i];
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }
    }
#if defined(AE_DEBUG)
    if ((shutter0 != pShutterCtrl[0])||
        (shutter1 != pShutterCtrl[1]))
//                (shutter2 != pShutterCtrl[2]))
    {
        AmbaPrint_PrintUInt5("[OV48C][SHRCONV]DCG Shutter: %d %d", pShutterCtrl[0], pShutterCtrl[1], 0, 0, 0);
        shutter0 = pShutterCtrl[0];
        shutter1 = pShutterCtrl[1];
//                shutter2 = pShutterCtrl[2];
    }
#endif
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pShutterCtrl);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_SetSlowShutterCtrl
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
static UINT32 OV48C40_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&SlowShutterCtrl);
    return RetVal;
}

static UINT32 OV48C40_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    AmbaMisra_TouchUnused(&pChan);
    AmbaMisra_TouchUnused(&pWbGainCtrl);
    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: OV48C40_Config
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
static UINT32 OV48C40_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 i, k;
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
    AMBA_VIN_MIPI_CPHY_PAD_CONFIG_s OV48C40PadConfig = {
        .DateRate   = 0U,
        .EnabledPin = EnabledPin[pChan->VinID]
    };
    AMBA_VIN_MIPI_VC_CONFIG_s MipiVirtChanConfig = {0};
    const UINT8 VCToVinID[2][4] = {
        [0] = {
            [0] = AMBA_VIN_CHANNEL0,
            [1] = AMBA_VIN_CHANNEL1,
            [2] = AMBA_VIN_CHANNEL2,
            [3] = AMBA_VIN_CHANNEL3,
        },
        [1] = {
            [0] = AMBA_VIN_CHANNEL4,
            [1] = AMBA_VIN_CHANNEL5,
            [2] = AMBA_VIN_CHANNEL6,
            [3] = AMBA_VIN_CHANNEL7,
        },
    };

    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 SensorMode = pMode->ModeID;
    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OV48C40Ctrl.Status.ModeInfo;

    if (SensorMode >= OV48C40_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_PrintUInt5("[OV48C40] Config Mode: %d(VinID: %d)", SensorMode, pChan->VinID, 0U, 0U, 0U);
        /* Update status & FrameTime */
        OV48C40_PrepareModeInfo(pMode, pModeInfo);

        /* Adjust mipi-phy parameters */
        OV48C40PadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        RetVal |= AmbaVIN_MipiCphyReset(pChan->VinID, &OV48C40PadConfig);
        /* INCK start */
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);

        RetVal |= AmbaKAL_TaskSleep(3);
        /* XCLR rising: hardware standby to software standy */
        /* Reset sensor */
        OV48C40_HardwareReset(pChan->VinID);

        /* Write registers of mode change to sensor */
        RetVal |= OV48C40_ChangeReadoutMode(pChan, SensorMode);

        /* Standby cancel */
        OV48C40_SetStandbyOff(pChan);

        /* Add delay to wait MIPI data output from sending standby cancel */
        /* T8 = 4.0 ms + The delay of the coarse integration time value */
        RetVal |= AmbaKAL_TaskSleep(5);

        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL) {
            k = (pChan->VinID == AMBA_VIN_CHANNEL0) ? 0U : 1U;

            for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i++) {
                 OV48C40_ConfigVin(VCToVinID[k][i], pModeInfo);
                 MipiVirtChanConfig.VirtChanMask = 0U;
                 MipiVirtChanConfig.VirtChan = (UINT8)i;
                 RetVal = AmbaVIN_MipiVirtChanConfig(VCToVinID[k][i], &MipiVirtChanConfig);
                 if (RetVal != ERR_NONE) {
                     break;
                 }
            }
        } else {
            OV48C40_ConfigVin(pChan->VinID, pModeInfo);
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_OV48C40Obj = {
    .Init                   = OV48C40_Init,
    .Enable                 = OV48C40_Enable,
    .Disable                = OV48C40_Disable,
    .Config                 = OV48C40_Config,
    .GetStatus              = OV48C40_GetStatus,
    .GetModeInfo            = OV48C40_GetModeInfo,
    .GetDeviceInfo          = OV48C40_GetDeviceInfo,
    .GetCurrentGainFactor   = OV48C40_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = OV48C40_GetCurrentShutterSpeed,

    .ConvertGainFactor      = OV48C40_ConvertGainFactor,
    .ConvertShutterSpeed    = OV48C40_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = OV48C40_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = OV48C40_SetDigitalGainCtrl,
    .SetShutterCtrl         = OV48C40_SetShutterCtrl,
    .SetWbGainCtrl          = OV48C40_SetWbGainCtrl,
    .SetSlowShutterCtrl     = OV48C40_SetSlowShutterCtrl,

    .RegisterRead           = OV48C40_RegisterRead,
    .RegisterWrite          = OV48C40_RegisterWrite,
    .GetHdrInfo             = OV48C40_GetHdrInfo,
    .ConfigPost             = NULL,

    .GetSerdesStatus        = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
