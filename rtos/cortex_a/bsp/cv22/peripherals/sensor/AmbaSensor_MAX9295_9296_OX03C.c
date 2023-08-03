/**
 *  @file AmbaSensor_MAX9295_9296_OX03C.c
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
 *  @details Control APIs of MAX9295_9296 plus OV OX03C CMOS sensor with MIPI interface
 *
 */

#include <AmbaWrap.h>
#include "AmbaVIN.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_9296_OX03C.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaSbrg_Max9295_9296.h"
#include "bsp.h"
#define MX00_OX03C_IN_SLAVE_MODE
#define MX00_OX03C_SERDES_ENABLE
static UINT16 SMode;


//#include "AmbaUtility.h"
static UINT32 MX00_OX03CI2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0]  = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1]  = AMBA_SENSOR_I2C_CHANNEL_PIP,
    //[AMBA_VIN_CHANNEL2]  = AMBA_SENSOR_I2C_CHANNEL_PIP2,
    //[AMBA_VIN_CHANNEL3]  = AMBA_SENSOR_I2C_CHANNEL_PIP3,
    //[AMBA_VIN_CHANNEL4]  = AMBA_SENSOR_I2C_CHANNEL_PIP4,
    //[AMBA_VIN_CHANNEL5]  = AMBA_SENSOR_I2C_CHANNEL_PIP5,
    //[AMBA_VIN_CHANNEL6]  = AMBA_SENSOR_I2C_CHANNEL_PIP6,
    //[AMBA_VIN_CHANNEL7]  = AMBA_SENSOR_I2C_CHANNEL_PIP7,
    //[AMBA_VIN_CHANNEL8]  = AMBA_SENSOR_I2C_CHANNEL_PIP8,
    //[AMBA_VIN_CHANNEL9]  = AMBA_SENSOR_I2C_CHANNEL_PIP9,
    //[AMBA_VIN_CHANNEL10] = AMBA_SENSOR_I2C_CHANNEL_PIP10,
    //[AMBA_VIN_CHANNEL11] = AMBA_SENSOR_I2C_CHANNEL_PIP11,
    //[AMBA_VIN_CHANNEL12] = AMBA_SENSOR_I2C_CHANNEL_PIP12,
    //[AMBA_VIN_CHANNEL13] = AMBA_SENSOR_I2C_CHANNEL_PIP13,
};


#ifdef MX00_OX03C_IN_SLAVE_MODE
static UINT32 MX00_OX03CMasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL1]  = AMBA_SENSOR_MSYNC_CHANNEL_PIP,
    //[AMBA_VIN_CHANNEL2]  = AMBA_SENSOR_MSYNC_CHANNEL_PIP2,
    //[AMBA_VIN_CHANNEL3]  = AMBA_SENSOR_MSYNC_CHANNEL_PIP3,
    //[AMBA_VIN_CHANNEL4]  = AMBA_SENSOR_MSYNC_CHANNEL_PIP4,
    //[AMBA_VIN_CHANNEL5]  = AMBA_SENSOR_MSYNC_CHANNEL_PIP5,
    //[AMBA_VIN_CHANNEL6]  = AMBA_SENSOR_MSYNC_CHANNEL_PIP6,
    //[AMBA_VIN_CHANNEL7]  = AMBA_SENSOR_MSYNC_CHANNEL_PIP7,
    //[AMBA_VIN_CHANNEL8]  = AMBA_SENSOR_MSYNC_CHANNEL_PIP8,
    //[AMBA_VIN_CHANNEL9]  = AMBA_SENSOR_MSYNC_CHANNEL_PIP9,
    //[AMBA_VIN_CHANNEL10] = AMBA_SENSOR_MSYNC_CHANNEL_PIP10,
    //[AMBA_VIN_CHANNEL11] = AMBA_SENSOR_MSYNC_CHANNEL_PIP11,
    //[AMBA_VIN_CHANNEL12] = AMBA_SENSOR_MSYNC_CHANNEL_PIP12,
    //[AMBA_VIN_CHANNEL13] = AMBA_SENSOR_MSYNC_CHANNEL_PIP13,
};
#endif

/*-----------------------------------------------------------------------------------------------*\
 * MX00_OX03C sensor runtime info
\*-----------------------------------------------------------------------------------------------*/
static MX00_OX03C_CTRL_s MX00_OX03CCtrl[AMBA_NUM_VIN_CHANNEL] = {0};

#ifdef MX00_OX03C_SERDES_ENABLE
static MAX9295_9296_SERDES_CONFIG_s MX00_OX03CSerdesConfig[AMBA_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01U,
        .SensorSlaveID   = MX00_OX03C_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] = (UINT8)(MX00_OX03C_I2C_SLAVE_ADDRESS + 0x10U),
            [1] = (UINT8)(MX00_OX03C_I2C_SLAVE_ADDRESS + 0x12U),
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},

        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    [1] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_OX03C_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] =  (UINT8)(MX00_OX03C_I2C_SLAVE_ADDRESS + 0x14U),
            [1] =  (UINT8)(MX00_OX03C_I2C_SLAVE_ADDRESS + 0x16U),
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
        .HighBandwidthModeEn    = 0U,
    },
    ////VC - VIN1/2/3
    //[1] = {0},
    //[2] = {0},
    //[3] = {0},
    //[4] = {
    //    .EnabledLinkID   = 0x01U,
    //    .SensorSlaveID   = MX00_OX03C_I2C_SLAVE_ADDRESS,
    //    .SensorAliasID   = {
    //        [0] = (UINT8)(MX00_OX03C_I2C_SLAVE_ADDRESS + 0x14U),
    //        [1] = (UINT8)(MX00_OX03C_I2C_SLAVE_ADDRESS + 0x16U),
    //    },
    //    .NumDataBits     = 12,
    //    .CSIRxLaneNum    = 4,
    //    .CSITxLaneNum    = {4, 4},
    //
    //    .CSITxSpeed      = {10, 10},
    //},
    ////VC - VIN5/6/7
    //[5] = {0},
    //[6] = {0},
    //[7] = {0},
    //[8] = {
    //    .EnabledLinkID   = 0x01U,
    //    .SensorSlaveID   = MX00_OX03C_I2C_SLAVE_ADDRESS,
    //    .SensorAliasID   = {
    //        [0] = (UINT8)(MX00_OX03C_I2C_SLAVE_ADDRESS + 0x20U),
    //        [1] = (UINT8)(MX00_OX03C_I2C_SLAVE_ADDRESS + 0x22U),
    //    },
    //    .NumDataBits     = 12,
    //    .CSIRxLaneNum    = 4,
    //    .CSITxLaneNum    = {4, 4},
    //
    //    .CSITxSpeed      = {10, 10},
    //},
    ////VC - VIN9/10
    //[9] = {0},
    //[10] = {0},
    //[11] = {
    //    .EnabledLinkID   = 0x01U,
    //    .SensorSlaveID   = MX00_OX03C_I2C_SLAVE_ADDRESS,
    //    .SensorAliasID   = {
    //        [0] = (UINT8)(MX00_OX03C_I2C_SLAVE_ADDRESS + 0x24U),
    //        [1] = (UINT8)(MX00_OX03C_I2C_SLAVE_ADDRESS + 0x26U),
    //    },
    //    .NumDataBits     = 12,
    //    .CSIRxLaneNum    = 4,
    //    .CSITxLaneNum    = {4, 4},
    //
    //    .CSITxSpeed      = {10, 10},
    //},
};
#else
static MAX9295_9296_SERDES_CONFIG_s MX00_OX03CSerdesConfig[MX00_OX03C_NUM_VIN_CHANNEL] = {
    [0] = {
        .EnabledLinkID   = 0x01,
        .SensorSlaveID   = MX00_OX03C_I2C_SLAVE_ADDRESS,
        .SensorAliasID   = {
            [0] =  (UINT8)(MX00_OX03C_I2C_SLAVE_ADDRESS),
            [1] =  (UINT8)(MX00_OX03C_I2C_SLAVE_ADDRESS),
        },
        .NumDataBits     = 12,
        .CSIRxLaneNum    = 4,
        .CSITxLaneNum    = {4, 4},
        .CSITxSpeed      = {10, 10},
    },
};

#endif


#ifdef MX00_OX03C_SERDES_ENABLE
static UINT16 MX00_OX03C_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 EnabledLinkID = 0U, i;

    for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }
    AmbaPrint_PrintUInt5("--- MX00_OX03C_GetEnabledLinkID, SensorID : %d, EnabledLinkID : %d ---", SensorID, EnabledLinkID, 0U, 0U, 0U);
    return (UINT16)EnabledLinkID;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_GetNumActiveSensor
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
static UINT32 MX00_OX03C_GetNumActiveSensor(UINT32 SensorID)
{
    UINT32 SensorNum = 0U, i;

    for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i ++) {
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
 *  @RoutineName:: MX00_OX03C_PrepareModeInfo
 *
 *  @Description:: Get sensor mode information
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *      Mode:   Sensor readout mode number
 *
 *  @Output     ::
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX00_OX03C_PrepareModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 SensorMode = pMode->ModeID;
    const AMBA_SENSOR_INPUT_INFO_s *pInputInfo = &MX00_OX03C_InputInfo[SensorMode];
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = &MX00_OX03C_OutputInfo[SensorMode];
    const MX00_OX03C_SENSOR_INFO_s *pSensorInfo = &MX00_OX03C_SensorInfo[SensorMode];
    const MX00_OX03C_FRAME_TIMING_s *pFrameTime = &MX00_OX03C_ModeInfoList[SensorMode].FrameTime;
    UINT32 SensorNum = MX00_OX03C_GetNumActiveSensor(pChan->SensorID);
    //DOUBLE FloorVal, DoubleTmp;
    DOUBLE Tmp;
    UINT32 U32RVal = 0;

    pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;
    //pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;
    if (SensorNum != 0U) {
        if (MX00_OX03C_HdrInfo[SensorMode].HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) {
            pModeInfo->FrameLengthLines = pSensorInfo->FrameLengthLines*2U;

        } else {
            pModeInfo->FrameLengthLines = pSensorInfo->FrameLengthLines;
        }
        pModeInfo->FrameLengthLines = pModeInfo->FrameLengthLines * SensorNum;
        //pModeInfo->NumExposureStepPerFrame = pSensorInfo->FrameLengthLines;
        pModeInfo->InternalExposureOffset = 0.0f;
        pModeInfo->RowTime = MX00_OX03C_ModeInfoList[SensorMode].RowTime / (FLOAT)SensorNum; //For OX03C row time is double row time
        //(void) AmbaWrap_floor((((DOUBLE)MX00_OX03C_ModeInfoList[SensorMode].PixelRate * (DOUBLE)pModeInfo->RowTime) + 0.5), &FloorVal);
        //DoubleTmp = (DOUBLE)MX00_OX03C_ModeInfoList[SensorMode].PixelRate * (DOUBLE)pModeInfo->RowTime;
        //pModeInfo->LineLengthPck = (UINT32)DoubleTmp;
        Tmp = ((DOUBLE)MX00_OX03C_ModeInfoList[SensorMode].PixelRate/(DOUBLE)pModeInfo->FrameLengthLines)* ((DOUBLE)pSensorInfo->LineLengthPck*(DOUBLE)pSensorInfo->FrameLengthLines)/90000000.0f;
        pModeInfo->LineLengthPck = (UINT32)Tmp / 2U; //single row
        pModeInfo->InputClk = pSensorInfo->InputClk;

        U32RVal |= AmbaWrap_memcpy(&pModeInfo->Config, pMode, sizeof(AMBA_SENSOR_CONFIG_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->FrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->InputInfo, pInputInfo, sizeof(AMBA_SENSOR_INPUT_INFO_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->OutputInfo, pOutputInfo, sizeof(AMBA_SENSOR_OUTPUT_INFO_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->MinFrameRate, &pFrameTime->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
        U32RVal |= AmbaWrap_memcpy(&pModeInfo->HdrInfo, &MX00_OX03C_HdrInfo[SensorMode], sizeof(AMBA_SENSOR_HDR_INFO_s)); // HDR information


        /* Updated minimum frame rate limitation */
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            pModeInfo->MinFrameRate.NumUnitsInTick /= 8U;
        }
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

    if(U32RVal != 0U) {
        /* MisraC */
    }
}
/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: MX00_OX03C_ConfigMasterSync
*
*  @Description:: Configure Master Sync generation
*
*  @Input      ::
*      pFrameTime: Sensor frame time configuration
*
*  @Output     :: none
*
*  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/

#ifdef MX00_OX03C_IN_SLAVE_MODE
static UINT32 MX00_OX03C_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID)
{
    static AMBA_VIN_MASTER_SYNC_CONFIG_s MX00_OX03CMasterSyncConfig = {
        .RefClk = 0,
        .HSync = {
            .Period     = 0,
            .PulseWidth = 0,
            .Polarity   = 0
        },
        .VSync = {
            .Period     = 0,
            .PulseWidth = 0,
            .Polarity   = 0
        },
        .HSyncDelayCycles = 0,
        .VSyncDelayCycles = 0,
        .ToggleHsyncInVblank = 1
    };

    UINT32 RetVal;
    AMBA_VIN_MASTER_SYNC_CONFIG_s *pMasterSyncCfg = &MX00_OX03CMasterSyncConfig;
    const MX00_OX03C_MODE_INFO_s *pModeInfo = &MX00_OX03C_ModeInfoList[ModeID];
    DOUBLE PeriodInDb;
    UINT32 U32RVal = 0U;

    U32RVal |= AmbaWrap_floor((((DOUBLE)pModeInfo->FrameTime.FrameRate.NumUnitsInTick * ((DOUBLE)MX00_OX03C_SensorInfo[ModeID].InputClk / (DOUBLE)pModeInfo->FrameTime.FrameRate.TimeScale)) + 0.5), &PeriodInDb);
    pMasterSyncCfg->RefClk = MX00_OX03C_SensorInfo[ModeID].InputClk;
    pMasterSyncCfg->HSync.Period = (UINT32)PeriodInDb;
    /*pMasterSyncCfg->HSync.PulseWidth = 8U;
    pMasterSyncCfg->HSync.Polarity   = 1U;
    pMasterSyncCfg->VSync.Period     = 1U;
    pMasterSyncCfg->VSync.PulseWidth = 1000U;
    pMasterSyncCfg->VSync.Polarity   = 1U;*/
    pMasterSyncCfg->HSync.PulseWidth  = 8U;
    pMasterSyncCfg->HSync.Polarity    = 1U;
    pMasterSyncCfg->VSync.Period      = 1U;
    pMasterSyncCfg->VSync.PulseWidth  = 1000U;
    pMasterSyncCfg->VSync.Polarity    = 1U;
    pMasterSyncCfg->ToggleHsyncInVblank  = 1U;
    RetVal = AmbaVIN_MasterSyncEnable(MX00_OX03CMasterSyncChannel[pChan->VinID], pMasterSyncCfg);

    if (RetVal == VIN_ERR_NONE) {
        RetVal = SENSOR_ERR_NONE;
    } else {
        RetVal = SENSOR_ERR_INVALID_API;
    }

    if(U32RVal != 0U){
        /* MisraC */
    }

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_ConfigSerDes
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
#ifdef MX00_OX03C_SERDES_ENABLE
static void MX00_OX03C_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 SensorMode, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;
    const MX00_OX03C_SENSOR_INFO_s *pSensorInfo = &MX00_OX03C_SensorInfo[SensorMode];
    MAX9295_9296_SERDES_CONFIG_s *pSerdesCfg = &MX00_OX03CSerdesConfig[pChan->VinID];
    DOUBLE FloorVal;
    UINT32 U32RVal = 0U;

    pSerdesCfg->EnabledLinkID = MX00_OX03C_GetEnabledLinkID(pChan->SensorID);

    pSerdesCfg->NumDataBits = pSensorInfo->NumDataBits;
    pSerdesCfg->CSIRxLaneNum = pSensorInfo->NumDataLanes;
    pSerdesCfg->CSITxLaneNum[0] = pImgInfo->NumDataLanes;
    U32RVal |= AmbaWrap_floor((DOUBLE)pImgInfo->DataRate * 1e-8, &FloorVal);
    pSerdesCfg->CSITxSpeed[0] = (UINT8)FloorVal;

    (void) Max9295_9296_Config(pChan->VinID, &MX00_OX03CSerdesConfig[pChan->VinID]);

    if(U32RVal != 0U){
        /* MisraC */
    }
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_ConfigVin
 *
 *  @Description:: Configure VIN to receive output frames of the new readout mode
 *
 *  @Input      ::
 *      pChan:       Vin ID and sensor ID
 *      pModeInfo:  Details of the specified readout mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_ConfigVin(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    /*-----------------------------------------------------------------------------------------------*\
     * Configurations for Amba. VIN side
    \*-----------------------------------------------------------------------------------------------*/
    static AMBA_VIN_MIPI_CONFIG_s MX00_OX03CVinConfig = {
        .Config = {
            .FrameRate = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .ColorSpace         = AMBA_VIN_COLOR_SPACE_RGB,
            .BayerPattern       = AMBA_VIN_BAYER_PATTERN_RG,
            .NumDataBits        = 0,
            .NumSkipFrame       = 1U, /* wait for initialization period */
            .RxHvSyncCtrl = {
                .NumActivePixels    = 0,
                .NumActiveLines     = 0,
                .NumTotalPixels     = 0,
                .NumTotalLines      = 0,
            },
            .SplitCtrl          = {0},
            .DelayedVsync       = 0,
        },
        .NumActiveLanes         = 4U,
        .DataType               = 0x2CU,
        .DataTypeMask           = 0x1FU,//0x1fU,
        .VirtChanHDREnable      = 0U,
        .VirtChanHDRConfig      = {0},
    };

    const AMBA_SENSOR_OUTPUT_INFO_s  *pImgInfo = &pModeInfo->OutputInfo;

    AMBA_VIN_MIPI_CONFIG_s *pVinCfg = &MX00_OX03CVinConfig;
    UINT32 RetVal;
    UINT32 U32RVal = 0;

    pVinCfg->NumActiveLanes = pImgInfo->NumDataLanes;
    pVinCfg->Config.NumDataBits = pImgInfo->NumDataBits;
    pVinCfg->Config.BayerPattern = pImgInfo->BayerPattern;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels  = pImgInfo->OutputWidth;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines   = pImgInfo->OutputHeight;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels   = pModeInfo->LineLengthPck - 1U;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines    = pModeInfo->FrameLengthLines;
    //pVinCfg->VirtChanHDREnable = 1U;
    //pVinCfg->VirtChanHDRConfig.VirtChanNum = 1U;
    //pVinCfg->VirtChanHDRConfig.VCPattern1stExp = 0U;
    //pVinCfg->VirtChanHDRConfig.VCPattern2ndExp = 0U;
    //pVinCfg->VirtChanHDRConfig.VCPattern2ndExp = 0U;;
    //pVinCfg->VirtChanHDRConfig.VCPattern3rdExp = 0U;;
    //pVinCfg->VirtChanHDRConfig.VCPattern4thExp = 0U;;
//
    //pVinCfg->VirtChanHDRConfig.Offset2ndExp = 0U;;
    //pVinCfg->VirtChanHDRConfig.Offset3rdExp = 0U;;
    //pVinCfg->VirtChanHDRConfig.Offset4thExp = 0U;;
    U32RVal |= AmbaWrap_memcpy(&pVinCfg->Config.FrameRate, &pModeInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "NumActiveLanes=%d,NumDataBits=%d, NumActivePixels=%d, NumActiveLines=%d, NumTotalPixels=%d ",
                               pVinCfg->NumActiveLanes, pVinCfg->Config.NumDataBits, pVinCfg->Config.RxHvSyncCtrl.NumActivePixels, pVinCfg->Config.RxHvSyncCtrl.NumActiveLines, pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels);
    RetVal = AmbaVIN_MipiConfig(pChan->VinID, pVinCfg);

    if (RetVal == VIN_ERR_NONE) {
        RetVal = SENSOR_ERR_NONE;
    } else {
        RetVal = SENSOR_ERR_INVALID_API;
    }

    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IMX290_MIPI_HardwareReset
 *
 *  @Description:: Reset IMX290_MIPI Image Sensor Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_HardwareReset(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
#ifdef MX00_OX03C_SERDES_ENABLE
    if ((pChan->SensorID & MX00_OX03C_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    if ((pChan->SensorID & MX00_OX03C_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_GPIO_PIN_4, 0);
    }
    (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
    if ((pChan->SensorID & MX00_OX03C_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= Max9295_9296_SetGpioOutput(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_GPIO_PIN_4, 1);
    }
    if ((pChan->SensorID & MX00_OX03C_SENSOR_ID_CHAN_B) != 0U) {
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
    (void)AmbaKAL_TaskSleep(30);
#endif
    if (RetVal != MAX9295_9296_ERR_NONE) {
        RetVal = SENSOR_ERR_SERDES;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_RegRead
 *
 *  @Description:: Read sensor registers
 *
 *  @Input      ::
 *      pChan:      Vin ID and sensor ID
 *      Addr:      Register Address
 *
 *  @Output     ::
 *      pRxData:   Pointer to Read data buffer
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_RegRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    if ((pChan->SensorID != MX00_OX03C_SENSOR_ID_CHAN_A) && (pChan->SensorID != MX00_OX03C_SENSOR_ID_CHAN_B)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cTxConfig.DataSize  = 2U;
        I2cTxConfig.pDataBuf  = TxData;
        TxData[0] = (UINT8)(Addr >> 8U);
        TxData[1] = (UINT8)(Addr & 0xffU);

        I2cRxConfig.DataSize  = 1U;
        I2cRxConfig.pDataBuf  = pRxData;

        if (pChan->SensorID == MX00_OX03C_SENSOR_ID_CHAN_A) {
            I2cTxConfig.SlaveAddr = MX00_OX03CSerdesConfig[pChan->VinID].SensorAliasID[0];
            I2cRxConfig.SlaveAddr = (MX00_OX03CSerdesConfig[pChan->VinID].SensorAliasID[0] | (UINT32)1U);
        } else {
            I2cTxConfig.SlaveAddr = MX00_OX03CSerdesConfig[pChan->VinID].SensorAliasID[1];
            I2cRxConfig.SlaveAddr = (MX00_OX03CSerdesConfig[pChan->VinID].SensorAliasID[1] | (UINT32)1U);
        }

        /*        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x, 0x%x", I2cTxConfig.SlaveAddr, I2cRxConfig.SlaveAddr, 0U, 0U, 0U); */
        RetVal = AmbaI2C_MasterReadAfterWrite(MX00_OX03CI2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST, 1U, &I2cTxConfig,
                                              &I2cRxConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MX00_OX03C] Addr 0x%04x I2C does not work!!!!!", Addr, 0U, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_RegWrite
 *
 *  @Description:: Write sensor registers
 *
 *  @Input      ::
 *      pChan:      Vin ID and sensor ID
 *      Addr:      Register Address
 *      pTxData:   Pointer to write data
 *      Size:      Number of registers to write
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_RegWriteSeq(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, const UINT8 *pData, UINT32 Size)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[MX00_OX03C_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize, i;
#ifdef DEBUG_MSG
    UINT32 k;
#endif

    if ((pChan == NULL) || (Size > MX00_OX03C_SENSOR_I2C_MAX_SIZE)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.DataSize  = (2U + Size);
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 2U] = pData[i];
        }

#if 0  /* to be refined with bsp file */
        RetVal = AmbaI2C_MasterWrite(MX00_OX03CI2cChannel[pChan->VinID], AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);
#endif
        if (pChan->SensorID == (MX00_OX03C_SENSOR_ID_CHAN_A | MX00_OX03C_SENSOR_ID_CHAN_B)) {
            I2cConfig.SlaveAddr = MX00_OX03CSerdesConfig[pChan->VinID].SensorSlaveID;

            /* AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U); */
            RetVal = AmbaI2C_MasterWrite(MX00_OX03CI2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                         &I2cConfig, &TxSize, 1000U);
#ifdef DEBUG_MSG
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[Broadcast] Slave ID 0x%2x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U);
            for (k = 0U; k < Size; k++) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Addr 0x%04x Data 0x%02x", Addr+k, TxDataBuf[k + 2U], 0U, 0U, 0U);
            }
#endif

        } else {
            for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = MX00_OX03CSerdesConfig[pChan->VinID].SensorAliasID[i];
                    /* AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U); */
                    RetVal |= AmbaI2C_MasterWrite(MX00_OX03CI2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
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
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_OX03C] Addr 0x%04x Size %d I2C does not work!!!!!", Addr, Size, 0U, 0U, 0U);
        }
    }

    //SENSOR_DBG_IO("[MX00_OX03C][IO] Addr = 0x%04x, Data = 0x%02x", Addr, Data);

    return RetVal;
}

static UINT32 MX00_OX03C_RegWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[MX00_OX03C_SENSOR_I2C_MAX_SIZE + 2U];
    UINT32 TxSize, i;


    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        I2cConfig.DataSize  = (2U + 1U);
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8) ((Addr & 0xff00U) >> 8U);
        TxDataBuf[1] = (UINT8) (Addr & 0x00ffU);
        TxDataBuf[2] = (UINT8) Data;


        if (pChan->SensorID == (MX00_OX03C_SENSOR_ID_CHAN_A | MX00_OX03C_SENSOR_ID_CHAN_B)) {
            I2cConfig.SlaveAddr = MX00_OX03CSerdesConfig[pChan->VinID].SensorSlaveID;

            /* AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U); */
            RetVal = AmbaI2C_MasterWrite(MX00_OX03CI2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                         &I2cConfig, &TxSize, 1000U);
        } else {
            for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i ++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
                    I2cConfig.SlaveAddr = MX00_OX03CSerdesConfig[pChan->VinID].SensorAliasID[i];
                    /* AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "slave addr: 0x%x", I2cConfig.SlaveAddr, 0U, 0U, 0U, 0U); */
                    RetVal |= AmbaI2C_MasterWrite(MX00_OX03CI2cChannel[pChan->VinID], AMBA_I2C_SPEED_FAST,
                                                  &I2cConfig, &TxSize, 1000U);
                }
            }
        }
        if (RetVal != I2C_ERR_NONE) {
            RetVal = SENSOR_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_9296_OX03C] Addr 0x%04x Size %d I2C does not work!!!!!", Addr, 1U, 0U, 0U, 0U);
        }
    }

    ///{
    ///UINT8 rData;
    ///MX00_OX03C_RegRead(pChan, Addr, &rData);
    ///AmbaPrint_PrintUInt5("Addr=0x%04X, [Write]Data=0x%02X, [Read]Data=0x%02X", Addr, Data, rData, 0U, 0U);
    ///}

    return RetVal;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_RegisterRead
 *
 *  @Description:: Read sensor registers
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *      Addr:   Register address
 *
 *  @Output     ::
 *      pData:  Pointer to Read data buffer
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pData)
{
    UINT8 RData = 0xFFU;
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_OX03C_RegRead(pChan, Addr, &RData);
        *pData = RData;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_RegisterWrite
 *
 *  @Description:: Write sensor registers
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *      Addr:   Register address
 *      Data:   Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_OX03C_RegWrite(pChan, Addr, Data);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_SetAGainReg
 *
 *  @Description:: Configure sensor analog gain setting
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *      GainType : LCG:0, HCG:1
 *      PGC:    Programmable gain control setting
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_SetAGainReg( const AMBA_SENSOR_CHANNEL_s *pChan,const UINT32 *pAnalogGainCtrl)
{
    UINT8 TxData[4];
    UINT8 TxData2[4];
    UINT8 DgcMsb, DgcLsb;
    UINT16 tmp;
    UINT32 AGAIN;
    UINT32 DGAIN;
    UINT32 j;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT8 DgcH, DgcM, DgcL;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
    for (j = 0U; j<(pModeInfo->HdrInfo.ActiveChannels); j ++) {
        AGAIN = pAnalogGainCtrl[j] & 0x0000ffffU;
        DGAIN = (pAnalogGainCtrl[j] & 0xffff0000U) >> 16U;
        DgcH = (UINT8)(DGAIN >> 10U);
        DgcH = (DgcH & (0x0fU));
        DgcM = (UINT8)((DGAIN & 0x3ffU) >> 2U);
        DgcL = (UINT8)(DGAIN &(0x0fU));
        tmp = (UINT16)DgcL << 6U;
        DgcL = (UINT8) tmp;
        DgcMsb = (UINT8)(AGAIN >> 6U);
        DgcMsb = DgcMsb & 0x1fU;
        DgcLsb = (UINT8)(AGAIN &0x3fU);
        tmp = (UINT16)DgcLsb << 2U;
        DgcLsb = (UINT8)tmp;
        TxData[0] = DgcMsb;
        TxData[1] = DgcLsb;
        TxData2[0]= DgcH;
        TxData2[1]= DgcM;
        TxData2[2]= DgcL;
        if (j == 0U) { //HCG, R0x3508-3509
            RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_HCG_AGAIN, TxData, 2U);
            RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_HCG_DGAIN, TxData2, 3U);

        } else if(j == 1U) {
            //LCG, R0x3548-3549
            RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_LCG_AGAIN, TxData, 2U);
            RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_LCG_DGAIN, TxData2, 3U);

        } else if(j == 2U) { //VS
            RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_SPD_AGAIN, TxData, 2U);
            RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_SPD_DGAIN, TxData2, 3U);
        } else {
            RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_VS_AGAIN, TxData, 2U);
            RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_VS_DGAIN, TxData2, 3U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_SetDGainReg
 *
 *  @Description:: Configure sensor digital gain setting
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *      PGC:    Programmable gain control setting
 *      GainType: LCG : 0, HCG : 1
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
//static UINT32 MX00_OX03C_SetDGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 GainType, UINT32 DigitalGainCtrl)
//{
//    UINT32 RetVal = SENSOR_ERR_NONE;
//    //const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
//    UINT8 TxData[3];
//    UINT8 DgcH, DgcM, DgcL;
//    DgcH = (UINT8)(DigitalGainCtrl >> 10U);
//    DgcH = (DgcH & (0x0fU));
//    DgcM = (UINT8)((DigitalGainCtrl & 0x3ffU) >> 2U);
//    DgcL = (UINT8)(DigitalGainCtrl &(0x0fU));
//    DgcL = DgcL << 6U;
//
//    TxData[0]= DgcH;
//    TxData[1]= DgcM;
//    TxData[2]= DgcL;
//    if(pChan == NULL){}
//    //HCG, R0x350a-350c
//    if (GainType == 1U) {
//        RetVal |= MX00_OX03C_RegWriteSeq(pChan, 0x350a, TxData , 3U);
//    }else{
//        //LCG, R0x354a-354c
//        RetVal |= MX00_OX03C_RegWriteSeq(pChan, 0x354a, TxData , 3U);
//    }
//    //AmbaPrint_PrintUInt5("--- MX00_OX03C_SetDGainReg, GainType : %d ---", GainType, 0U, 0U, 0U, 0U);
//    return RetVal;
//}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_SetShutterReg
 *
 *  @Description:: Apply electronic shutter setting
 *
 *  @Input      ::
 *      pChan:            Vin ID and sensor ID
 *      NumXhsEshrSpeed: Integration time in number of H period
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_SetShutterReg(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ShutterCtrl, UINT8 EXP)
{
    UINT8  TxData[2];
    UINT32 RetVal = SENSOR_ERR_NONE;
    FLOAT ActualShutter;
    if(pChan == NULL) {
        /////
    }
    TxData[0]= (UINT8)((ShutterCtrl & 0xff00U) >> 8U);
    TxData[1]= (UINT8)((ShutterCtrl & 0x00ffU));

    if( EXP == 0U) {
        RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_DCG_SHR_REG, TxData, 2U);
        ActualShutter = (FLOAT)ShutterCtrl * MX00_OX03C_ModeInfoList[SMode].RowTime;
        if(ActualShutter >= 0.011f) {  // > 11ms turned off LFM weighting
            RetVal |= MX00_OX03C_RegWrite(pChan, 0x5D12, 0x00);
        } else {
            RetVal |= MX00_OX03C_RegWrite(pChan, 0x5D12, 0x10);
        }

    } else if(EXP == 2U) {
        RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_SPD_SHR_REG, TxData, 2U);
    } else if(EXP == 3U) {
        TxData[0]= 0U;
        TxData[1]= 0U;
        RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_VS_SHR_REG, TxData, 2U);
    } else {
        ///////
    }
    //AmbaPrint_PrintUInt5("--- OX03C_SetShutterReg ---", 0U, 0U, 0U, 0U, 0U);
    return RetVal;
}








/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_SetWbGainReg
 *
 *  @Description:: Apply WB gain setting
 *
 *  @Input      ::
 *      pChan:            Vin ID and sensor ID
 *      NumXhsEshrSpeed: Integration time in number of H period
 *      GainType : LCG:0, HCG:1
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_SetWbGainReg(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    //const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OX03CCtrl.Status.ModeInfo;
    UINT32 TxDataTmp[8];
    UINT8 TxData[8];
    if(pChan == NULL) {
        /////
    }
    TxDataTmp[0] = ((pWbGainCtrl->B >> 8U) & (UINT32)0x7f);
    TxDataTmp[1] = (pWbGainCtrl->B & (UINT32)0xff);
    TxDataTmp[2] = ((pWbGainCtrl->Gb >> 8U) & (UINT32)0x7f);
    TxDataTmp[3] = (pWbGainCtrl->Gb & (UINT32)0xff);
    TxDataTmp[4] = ((pWbGainCtrl->Gr >> 8U) & (UINT32)0x7f);
    TxDataTmp[5] = (pWbGainCtrl->Gr & (UINT32)0xff);
    TxDataTmp[6] = ((pWbGainCtrl->R >> 8U) & (UINT32)0x7f);
    TxDataTmp[7] = (pWbGainCtrl->R & (UINT32)0xff);

    TxData[0] = (UINT8)TxDataTmp[0];
    TxData[1] = (UINT8)TxDataTmp[1];
    TxData[2] = (UINT8)TxDataTmp[2];
    TxData[3] = (UINT8)TxDataTmp[3];
    TxData[4] = (UINT8)TxDataTmp[4];
    TxData[5] = (UINT8)TxDataTmp[5];
    TxData[6] = (UINT8)TxDataTmp[6];
    TxData[7] = (UINT8)TxDataTmp[7];

    RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_WBGAIN_HCG_B, TxData, 8U);
    RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_WBGAIN_LCG_B, TxData, 8U);
    RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_WBGAIN_SPD_B, TxData, 8U);
    RetVal |= MX00_OX03C_RegWriteSeq(pChan, MX00_OX03C_WBGAIN_VS_B, TxData, 8U);

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_GetHdrInfo
 *
 *  @Description:: Get Sensor HDR information
 *
 *  @Input      ::
 *      pChan:            Vin ID and sensor ID
 *      pShutterCtrl:    Pointer to electronic shutter control
 *
 *  @Output     ::
 *      pHdrInfo:   senosr hdr information
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;
    ////const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &OX03CCtrl[pChan->VinID].Status.ModeInfo;
    ////UINT16 SEFShtCtrl = (UINT16)pShutterCtrl[1];

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pHdrInfo, &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo.HdrInfo, sizeof(AMBA_SENSOR_HDR_INFO_s));
    }

    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_SetStandbyOn
 *
 *  @Description:: Configure sensor into standby mode
 *
 *  @Input      ::
 *      pChan:      Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_StandbyOn(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_OX03C_RegWrite(pChan, MX00_OX03C_STANDBY, 0x00);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_SetStandbyOff
 *
 *  @Description:: Configure sensor to leave standby mode
 *
 *  @Input      ::
 *      pChan:      Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_StandbyOff(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_OX03C_RegWrite(pChan, MX00_OX03C_STANDBY, 0x01);
    }

    return RetVal;
}




/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_ChangeReadoutMode
 *
 *  @Description:: Switch sensor to requested mode.
 *                 It's for the case that sensor have been initialized.
 *
 *  @Input      ::
 *    pChan:        Vin ID and sensor ID
 *    SensorMode:  Sensor mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_ChangeReadoutMode(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode)
{
    UINT32 i;
    UINT16 Addr;
    UINT16 Data;
    UINT32 RetVal = SENSOR_ERR_NONE;

    RetVal |= MX00_OX03C_RegWrite(pChan, MX00_OX03C_SW_RST, 0x01);// Sensor software reset
    (void)AmbaKAL_TaskSleep(10); // Delay 10ms after sensor software reset
    if(SensorMode == MX00_OX03C_1920_1280_SHDR4) {
        for (i = 0U; i < MX00_OX03C_NUM_READOUT_HDR4_REG; i++) {
            Addr = MX00_OX03C_HDR4Table[i].Addr;
            Data = MX00_OX03C_HDR4Table[i].Data;
            RetVal |= MX00_OX03C_RegWrite(pChan, Addr, Data);
        }
    } else if(SensorMode == MX00_OX03C_1920_1280_SHDR4_P30_NoVC) {
        for (i = 0U; i < MX00_OX03C_NUM_READOUT_HDR4_NoVC_REG; i++) {
            Addr = MX00_OX03C_HDR4NoVCTable[i].Addr;
            Data = MX00_OX03C_HDR4NoVCTable[i].Data;
            RetVal |= MX00_OX03C_RegWrite(pChan, Addr, Data);
        }
    } else if(SensorMode == MX00_OX03C_1920_1280_SHDR4_P60_NoVC) {
        for (i = 0U; i < MX00_OX03C_NUM_READOUT_HDR4_NoVC_REG; i++) {
            Addr = MX00_OX03C_HDR4NoVCTable[i].Addr;
            Data = MX00_OX03C_HDR4NoVCTable[i].Data;
            RetVal |= MX00_OX03C_RegWrite(pChan, Addr, Data);
        }
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x380e, 0x02);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x380f, 0xae);

    } else if(SensorMode == MX00_OX03C_1920_1080_SHDR4_P30_NoVC) {
        for (i = 0U; i < MX00_OX03C_NUM_READOUT_960M_HDR4_NoVC_REG; i++) {
            Addr = MX00_OX03C_960M_HDR4NoVCTable[i].Addr;
            Data = MX00_OX03C_960M_HDR4NoVCTable[i].Data;
            RetVal |= MX00_OX03C_RegWrite(pChan, Addr, Data);
        }
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3012, 0x21);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x380c, 0x0c);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x380d, 0x8f);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x384c, 0x03);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x384d, 0x1c);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x388c, 0x03);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x388d, 0x3a);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x4611, 0x10);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x4615, 0x10);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3803, 0x68);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3806, 0x04);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3807, 0xa7);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x380a, 0x04);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x380b, 0x38);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x380f, 0x48);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x380c, 0x0a);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x380d, 0x08);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x384c, 0x05);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x384d, 0x04);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x388c, 0x05);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x388d, 0x04);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3501, 0x00);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3502, 0xc1);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3541, 0x00);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3542, 0xc1);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3588, 0x01);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3589, 0x40);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x35c8, 0x01);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x35c9, 0x40);
    } else if(SensorMode == MX00_OX03C_1920_712_SHDR4_P60_NoVC) {
        for (i = 0U; i < MX00_OX03C_NUM_READOUT_960M_HDR4_NoVC_REG; i++) {
            Addr = MX00_OX03C_960M_HDR4NoVCTable[i].Addr;
            Data = MX00_OX03C_960M_HDR4NoVCTable[i].Data;
            RetVal |= MX00_OX03C_RegWrite(pChan, Addr, Data);
        }


    } else {
        //No this mode
    }
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x3820,    0x20); //mirror
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5e00,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E01,    0x9 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E02,    0x9 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E03,    0x9 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E04,    0x9 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E05,    0x9 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E06,    0x9 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E07,    0x9 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E08,    0x9 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E09,    0xa );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E0A,    0xa );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E0B,    0xa );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E0C,    0xa );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E0D,    0xb );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E0E,    0xb );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E0F,    0xb );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E10,    0xb );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E11,    0xc );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E12,    0xc );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E13,    0xc );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E14,    0xc );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E15,    0xd );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E16,    0xd );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E17,    0xe );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E18,    0xf );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E19,    0xf );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E1A,    0x10);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E1B,    0x10);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E1C,    0x12);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E1D,    0x13);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E1E,    0x14);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E1F,    0x15);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E20,    0x16);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E21,    0x17);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E22,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E23,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E24,    0xff);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E25,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E26,    0x1 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E27,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E28,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E29,    0x1 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E2A,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E2B,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E2C,    0x1 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E2D,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E2E,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E2F,    0x1 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E30,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E31,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E32,    0x1 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E33,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E34,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E35,    0x1 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E36,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E37,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E38,    0x1 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E39,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E3A,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E3B,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E3C,    0x80);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E3D,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E3E,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E3F,    0x80);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E40,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E41,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E42,    0x80);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E43,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E44,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E45,    0x80);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E46,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E47,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E48,    0x80);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E49,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E4A,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E4B,    0x80);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E4C,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E4D,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E4E,    0x80);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E4F,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E50,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E51,    0x80);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E52,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E53,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E54,    0x80);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E55,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E56,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E57,    0x80);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E58,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E59,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E5A,    0x40);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E5B,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E5C,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E5D,    0x40);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E5E,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E5F,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E60,    0x40);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E61,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E62,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E63,    0x40);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E64,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E65,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E66,    0x40);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E67,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E68,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E69,    0x40);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E6A,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E6B,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E6C,    0x40);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E6D,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E6E,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E6F,    0x40);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E70,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E71,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E72,    0x20);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E73,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E74,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E75,    0x20);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E76,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E77,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E78,    0x20);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E79,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E7A,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E7B,    0x20);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E7C,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E7D,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E7E,    0x20);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E7F,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E80,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E81,    0x20);
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E82,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E83,    0x0 );
    RetVal |= MX00_OX03C_RegWrite(pChan, 0x5E84,    0x40);
    //RetVal |= MX00_OX03C_RegWrite(pChan, 0x5b80,    0x09);//disable VS



    for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i++) {
        //if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
        /* reset gain/shutter ctrl information */
        MX00_OX03CCtrl[pChan->VinID].CurrentShrCtrl[i][0] = 0xffffffffU;
        MX00_OX03CCtrl[pChan->VinID].CurrentShrCtrl[i][1] = 0xffffffffU;
        MX00_OX03CCtrl[pChan->VinID].CurrentAGCtrl[i][0] = 0xffffffffU;
        MX00_OX03CCtrl[pChan->VinID].CurrentAGCtrl[i][1] = 0xffffffffU;
        MX00_OX03CCtrl[pChan->VinID].CurrentAGCtrl[i][2] = 0xffffffffU;
        MX00_OX03CCtrl[pChan->VinID].CurrentDGCtrl[i][0] = 0U;
        MX00_OX03CCtrl[pChan->VinID].CurrentDGCtrl[i][1] = 0U;
        MX00_OX03CCtrl[pChan->VinID].CurrentDGCtrl[i][2] = 0U;
        MX00_OX03CCtrl[pChan->VinID].CurrentGainType[i][0] = 0U;
        MX00_OX03CCtrl[pChan->VinID].CurrentGainType[i][1] = 0U;
        MX00_OX03CCtrl[pChan->VinID].CurrentGainType[i][2] = 0U;
        //}
    }
    return RetVal;
}

static UINT32 MX00_OX03C_SensorModeInit(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 SensorMode)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    RetVal |= MX00_OX03C_ChangeReadoutMode(pChan, SensorMode);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_PreCalculateModeInfo
 *
 *  @Description:: Calculate Mode Information in advance.
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void MX00_OX03C_PreCalculateModeInfo(void)
{
    UINT32 i;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;
    const MX00_OX03C_SENSOR_INFO_s *pSensorInfo = NULL;
    FLOAT LineLengthPck;

    for (i = 0U; i < MX00_OX03C_NUM_MODE; i++) {
        pSensorInfo = &MX00_OX03C_SensorInfo[i];
        //Need to check the RowTime calculation method
        //OX03C_ModeInfoList[i].RowTime = (FLOAT) ((DOUBLE) pSensorInfo->LineLengthPck /
        //                                                       ( (DOUBLE) pSensorInfo->DataRate *
        //                                                         (DOUBLE) pSensorInfo->NumDataLanes /
        //                                                         (DOUBLE) pSensorInfo->NumDataBits) );
        //OX03C_ModeInfoList[i].RowTime = (FLOAT) ((DOUBLE) pSensorInfo->LineLengthPck / 108000000.0);
        pOutputInfo = &MX00_OX03C_OutputInfo[i];
        MX00_OX03C_ModeInfoList[i].PixelRate = (FLOAT) ((DOUBLE) pOutputInfo->DataRate *
                                               (DOUBLE) pOutputInfo->NumDataLanes /
                                               (DOUBLE) pOutputInfo->NumDataBits);

        LineLengthPck = ((((FLOAT)MX00_OX03C_ModeInfoList[i].PixelRate / (FLOAT)pSensorInfo->FrameLengthLines) * (FLOAT)pSensorInfo->LineLengthPck) * (FLOAT)pSensorInfo->FrameLengthLines) / 90000000.0f;
        MX00_OX03C_ModeInfoList[i].RowTime = (FLOAT)  LineLengthPck / MX00_OX03C_ModeInfoList[i].PixelRate;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_Init
 *
 *  @Description:: Initialize sensor driver
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    const MX00_OX03C_SENSOR_INFO_s *pSensorInfo = &MX00_OX03C_SensorInfo[0];

//    const MX00_OX03C_SENSOR_INFO_s *pSensorInfo = &MX00_OX03C_SensorInfo[0];

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID,  pSensorInfo->InputClk);     /* The default vin reference clock frequency */
//        RetVal = AmbaUserVIN_SensorClkCtrl(pChan->VinID, pSensorInfo->InputClk);
        MX00_OX03C_PreCalculateModeInfo();
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_Enable
 *
 *  @Description:: Power on sensor
 *
 *  @Input      ::
 *      pChan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        //RetVal |= MX00_OX03C_HardwareReset(pChan);
    }
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "MX00_OX03C_Enable", 0U, 0U, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_Disable
 *
 *  @Description:: Power down sensor
 *
 *  @Input      ::
 *      pChan: Vin ID and sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        RetVal = MX00_OX03C_StandbyOn(pChan);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_GetStatus
 *
 *  @Description:: Get current sensor status
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pStatus:    pointer to current sensor status
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) || (pStatus == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pStatus, &MX00_OX03CCtrl[pChan->VinID].Status, sizeof(AMBA_SENSOR_STATUS_INFO_s));
    }

    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_GetModeInfo
 *
 *  @Description:: Get Mode Info of indicated mode
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *      Mode:   Sensor Readout Mode Number
 *
 *  @Output     ::
 *      pModeInfo: pointer to requested Mode Info
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    AMBA_SENSOR_CONFIG_s Config = {0};

    if ((pChan == NULL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        if (pMode->ModeID == AMBA_SENSOR_CURRENT_MODE) {
            Config.ModeID = MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo.Config.ModeID;
        } else {
            Config.ModeID = pMode->ModeID;
        }

        if (Config.ModeID >= MX00_OX03C_NUM_MODE) {
            RetVal = SENSOR_ERR_ARG;
        } else {
            MX00_OX03C_PrepareModeInfo(pChan, &Config, pModeInfo);
        }
    }
    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_GetDeviceInfo
 *
 *  @Description:: Get Sensor Device Info
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pDeviceInfo:    pointer to device info
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 U32RVal = 0;

    if ((pChan == NULL) ||(pDeviceInfo == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        U32RVal |= AmbaWrap_memcpy(pDeviceInfo, &MX00_OX03C_DeviceInfo, sizeof(AMBA_SENSOR_DEVICE_INFO_s));
    }

    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_GetCurrentGainFactor
 *
 *  @Description:: Get Sensor Current Gain Factor
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pGainFactor:    pointer to current gain factor
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    //const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
    UINT32 i;
    UINT32 CurrentGainCtrl, CurrentCGCtrl, Tmp;
    UINT32 RetVal = SENSOR_ERR_NONE;
    DOUBLE DoubleTmp;

    //(void)pModeInfo;
    if ((pChan == NULL) ||(pGainFactor == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i++) {
            //Need to have a if to check
            CurrentGainCtrl = MX00_OX03CCtrl[pChan->VinID].CurrentAGCtrl[i][0];
            CurrentCGCtrl = MX00_OX03CCtrl[pChan->VinID].CurrentDGCtrl[i][0];
            Tmp = CurrentGainCtrl * CurrentCGCtrl;
            DoubleTmp = ((DOUBLE)Tmp/64.0)/1024.0;
            *pGainFactor = (FLOAT)DoubleTmp;

            //(FLOAT) ((FLOAT)((CurrentGainCtrl * CurrentCGCtrl)/64.0)/1024.0);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_GetCurrentShutterSpeed
 *
 *  @Description:: Get Sensor Current Exposure Time
 *
 *  @Input      ::
 *      pChan:   Vin ID and sensor ID
 *
 *  @Output     ::
 *      pExposureTime:    pointer to current exposure time
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 i,k;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    //Need to have a if to check
                    pExposureTime[0] = pModeInfo->RowTime * (FLOAT)MX00_OX03CCtrl[pChan->VinID].CurrentShrCtrl[i][0];
                }
            }
        } else {
            for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    //Need to have a if to check
                    for (k = 0U; k < (pModeInfo->HdrInfo.ActiveChannels); k ++) {
                        pExposureTime[k] =(FLOAT)MX00_OX03CCtrl[pChan->VinID].CurrentShrCtrl[i][k]* pModeInfo->RowTime;

                    }
                }
            }
        }
    }
    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_ConvertGainFactor
 *
 *  @Description:: Convert gain factor to analog and digital gain control
 *
 *  @Input      ::
 *      pChan:               Vin ID and sensor ID
 *      DesiredFactor:      Desired gain factor
 *
 *  @Output     ::
 *      pActualFactor:      Achievable gain factor (12.20 fixed point)
 *      pAnalogGainCtrl:    Analog gain control for achievable gain factor
 *      pDigitalGainCtrl:   Digital gain control for achievable gain factor
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void ConvertGainFactor(FLOAT DesiredFactor, UINT32 *pActualFactor, UINT32 *pAnalogGainCtrl,UINT32 exposureNum)
{
    static const FLOAT AnalogGain_LookUpTable[64]= {
        1.0f,
        1.0625f,
        1.125f,
        1.1875f,
        1.25f,
        1.3125f,
        1.375f,
        1.4375f,
        1.5f,
        1.5625f,
        1.625f,
        1.6875f,
        1.75f,
        1.8125f,
        1.875f,
        1.9375f,
        2.0f,
        2.125f,
        2.25f,
        2.375f,
        2.5f,
        2.625f,
        2.75f,
        2.875f,
        3.0f,
        3.125f,
        3.25f,
        3.375f,
        3.5f,
        3.625f,
        3.75f,
        3.875f,
        4.0f,
        4.25f,
        4.5f,
        4.75f,
        5.0f,
        5.25f,
        5.5f,
        5.75f,
        6.0f,
        6.25f,
        6.5f,
        6.75f,
        7.0f,
        7.25f,
        7.5f,
        7.75f,
        8.0f,
        8.5f,
        9.0f,
        9.5f,
        10.0f,
        10.5f,
        11.0f,
        11.5f,
        12.0f,
        12.5f,
        13.0f,
        13.5f,
        14.0f,
        14.5f,
        15.0f,
        15.5f,
    };
    //GainType, LCG : 0, HCG : 1
    UINT32 UTmp;
    UINT32 Agc;
    DOUBLE tmp, Again, Dgain;
    DOUBLE  FTmp;
    FLOAT  DesiredFactorTmp;
    UINT8  i;
    DesiredFactorTmp = DesiredFactor;
    if((exposureNum == 0U) ||(exposureNum == 1U) ||(exposureNum == 3U) ) {                       //HCG
        if (DesiredFactorTmp < MX00_OX03C_LPD_GAIN_MIN) {
            DesiredFactorTmp = MX00_OX03C_LPD_GAIN_MIN;
        }
        //DesiredFactorTmp /= MX00_OX03C_CG_GAIN_RATIO;
    } else if(exposureNum == 2U) {                                     //SPD
        if (DesiredFactorTmp < MX00_OX03C_SPD_GAIN_MIN) {
            DesiredFactorTmp = MX00_OX03C_SPD_GAIN_MIN;
        }
    } else {
        ///

    }
    if (DesiredFactorTmp <= MX00_OX03C_DeviceInfo.MaxAnalogGainFactor) {
        //Dgc = 1U;
        for (i = 0U; i < 64U; i++) {
            if (DesiredFactorTmp <= AnalogGain_LookUpTable[i]) {
                break;
            }
        }
        if(i > 63U) {
            i = 63U;
        }
        if (DesiredFactorTmp < AnalogGain_LookUpTable[i]) {
            if (i >= 1U) {
                i--;
            }
        }
        Again = AnalogGain_LookUpTable[i];
    } else { // > Analog gain, compensate by Dgain
        Again = AnalogGain_LookUpTable[63];
    }
    Dgain = (DOUBLE)DesiredFactorTmp / Again;

    if (Dgain > MX00_OX03C_DeviceInfo.MaxDigitalGainFactor) {
        Dgain = MX00_OX03C_DeviceInfo.MaxDigitalGainFactor;
    }
    tmp = Again * 64.0f;
    *pAnalogGainCtrl = (UINT32)tmp;
    FTmp = Dgain * 1024.0f;
    //*pDigitalGainCtrl = (UINT32)FTmp;
    Agc = *pAnalogGainCtrl * (UINT32)FTmp;
    *pAnalogGainCtrl |= (((UINT32)FTmp) << 16U);
    UTmp = (1048576U / 64U )/ 1024U;
    UTmp = UTmp* Agc;
    /*if(exposureNum == 0U){
        tmp = (DOUBLE)UTmp * MX00_OX03C_CG_GAIN_RATIO;
        UTmp = (UINT32) tmp;
    }*/
    *pActualFactor = UTmp;
}

static UINT32 MX00_OX03C_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 ActualFactor;
    AMBA_SENSOR_WB_GAIN_FACTOR_s WbGain;
    FLOAT FTmp;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
    AMBA_SENSOR_GAIN_FACTOR_s DesiredFactor;
    UINT32  HdrType = pModeInfo->HdrInfo.HdrType;
    UINT32 i = 0;
    //OX03CCtrl[pChan->VinID].CurrentGainType;
    FLOAT  compensateR = 1.0f;
    DesiredFactor = *pDesiredFactor;
    if (HdrType == AMBA_SENSOR_HDR_NONE) { //Linear, LCG
        /*if (DesiredFactor.Gain[0] < MX00_OX03C_LCG_GAIN_MIN ){
            DesiredFactor.Gain[0] = MX00_OX03C_LCG_GAIN_MIN;
        }*/
        //GainType = 1;//DecideLinearGainType(DesiredFactor.Gain[0]); //Need to add
        //ConvertGainFactor(GainType, DesiredFactor.Gain[0], &ActualFactor, &pGainCtrl->AnalogGain[0]);
        //pActualFactor->Gain[0] = (((FLOAT)ActualFactor) / 1024.0f ) / 1024.0f;
    } else { //HDR mode, DCG
        for (i = 0U; i < (pModeInfo->HdrInfo.ActiveChannels); i++) {
            ConvertGainFactor(DesiredFactor.Gain[i], &ActualFactor, &pGainCtrl->AnalogGain[i],i); //HCG
            pActualFactor->Gain[i] = (((FLOAT)ActualFactor) / 1024.0f ) / 1024.0f;
            //WB Gain


            if(pDesiredFactor->WbGain[0].R <= 1.0f) {
                compensateR = pDesiredFactor->WbGain[0].R;
            } else {
                ////
            }

            WbGain.R  = (pDesiredFactor->WbGain[0].R <= 1.0f)  ? 1.0f : pDesiredFactor->WbGain[0].R;
            WbGain.Gr = (pDesiredFactor->WbGain[0].Gr <= 1.0f) ? 1.0f : pDesiredFactor->WbGain[0].Gr;
            WbGain.Gb = (pDesiredFactor->WbGain[0].Gb <= 1.0f) ? 1.0f : pDesiredFactor->WbGain[0].Gb;
            WbGain.B  = (pDesiredFactor->WbGain[0].B <= 1.0f)  ? 1.0f : pDesiredFactor->WbGain[0].B;
            WbGain.Gr /= compensateR;
            WbGain.Gb /= compensateR;
            WbGain.B /= compensateR;
            WbGain.R  = (pDesiredFactor->WbGain[i].R  >= MX00_OX03C_WBGain_Max) ? MX00_OX03C_WBGain_Max : pDesiredFactor->WbGain[i].R;
            WbGain.Gr = (pDesiredFactor->WbGain[i].Gr >= MX00_OX03C_WBGain_Max) ? MX00_OX03C_WBGain_Max : pDesiredFactor->WbGain[i].Gr;
            WbGain.Gb = (pDesiredFactor->WbGain[i].Gb >= MX00_OX03C_WBGain_Max) ? MX00_OX03C_WBGain_Max : pDesiredFactor->WbGain[i].Gb;
            WbGain.B  = (pDesiredFactor->WbGain[i].B  >= MX00_OX03C_WBGain_Max) ? MX00_OX03C_WBGain_Max : pDesiredFactor->WbGain[i].B;
            //WbGain.R  = (pDesiredFactor->WbGain[i].R  <= MX00_OX03C_WBGain_Min) ? MX00_OX03C_WBGain_Min : WbGain.R;
            //WbGain.Gr = (pDesiredFactor->WbGain[i].Gr <= MX00_OX03C_WBGain_Min) ? MX00_OX03C_WBGain_Min : WbGain.Gr;
            //WbGain.Gb = (pDesiredFactor->WbGain[i].Gb <= MX00_OX03C_WBGain_Min) ? MX00_OX03C_WBGain_Min : WbGain.Gb;
            //WbGain.B  = (pDesiredFactor->WbGain[i].B  <= MX00_OX03C_WBGain_Min) ? MX00_OX03C_WBGain_Min : WbGain.B;
            /* Update */
            /*if(i == 2U){
               WbGain.R *= 0.8f;
               WbGain.B *= 0.92f;
            }*/
            FTmp = WbGain.R  * 1024.0f;
            pGainCtrl->WbGain[i].R  = (UINT32)FTmp;

            FTmp = WbGain.Gr * 1024.0f;
            pGainCtrl->WbGain[i].Gr = (UINT32)FTmp;
            FTmp = WbGain.Gb * 1024.0f;
            pGainCtrl->WbGain[i].Gb = (UINT32)FTmp;
            FTmp = WbGain.B * 1024.0f;
            pGainCtrl->WbGain[i].B  = (UINT32)FTmp;
            //AmbaPrint_PrintUInt5("--- ConvertGainFactor i%d %d  %d  %d  %d---", i, pGainCtrl->WbGain[i].B, pGainCtrl->WbGain[i].Gr,  pGainCtrl->WbGain[i].Gb,  pGainCtrl->WbGain[i].R);
            pActualFactor->WbGain[i].R  = (FLOAT)pGainCtrl->WbGain[i].R  / 1024.0f;
            pActualFactor->WbGain[i].Gr = (FLOAT)pGainCtrl->WbGain[i].Gr / 1024.0f;
            pActualFactor->WbGain[i].Gb = (FLOAT)pGainCtrl->WbGain[i].Gb / 1024.0f;
            pActualFactor->WbGain[i].B  = (FLOAT)pGainCtrl->WbGain[i].B  / 1024.0f;

        }


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
static void ConvertShutterSpeed(const AMBA_SENSOR_MODE_INFO_s *pModeInfo, UINT32 HdrChannel, FLOAT ExposureTime, FLOAT *pActualExptime, UINT32 *pShutterCtrl)
{
    //UINT32 NumExposureStepPerFrame = pModeInfo->NumExposureStepPerFrame;
    UINT32 FrameLengthLines = pModeInfo->FrameLengthLines;
    UINT32 ExposureLineMaxMargin = 10U;
    UINT32 MaxExposureLine, MinExposureLine;
    DOUBLE ShutterCtrlInDb;
    UINT32 ShutterCtrl;
    UINT32 U32RVal = 0U;

    U32RVal |= AmbaWrap_floor((DOUBLE)ExposureTime / (DOUBLE)pModeInfo->RowTime, &ShutterCtrlInDb);

    ShutterCtrl = (UINT32)ShutterCtrlInDb;

    /* For HDR mode (not support slow-shutter) */
    if (pModeInfo->HdrInfo.HdrType != AMBA_SENSOR_HDR_NONE) {
        MinExposureLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MinExposureLine;
        MaxExposureLine = pModeInfo->HdrInfo.ChannelInfo[HdrChannel].MaxExposureLine;
        /* For none-HDR mode (not support slow-shutter) */
    } else {
        MaxExposureLine = FrameLengthLines - ExposureLineMaxMargin;
        MinExposureLine = 1U;
    }

    if (ShutterCtrl > MaxExposureLine) {
        ShutterCtrl = MaxExposureLine;
    } else if (ShutterCtrl < MinExposureLine) {
        ShutterCtrl = MinExposureLine;
    } else {
        /* MisraC */  /* do not need to adjust ShutterCtrl */
    }

    *pShutterCtrl = ShutterCtrl;

    *pActualExptime = (FLOAT)*pShutterCtrl * pModeInfo->RowTime;

    //*pActualExptime = (FLOAT)ShutterCtrl * (pModeInfo->RowTime * ((FLOAT)FrameLengthLines / (FLOAT)NumExposureStepPerFrame));

    if(U32RVal != 0U){
        /* MisraC */
    }

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_ConvertShutterSpeed
 *
 *  @Description:: Convert exposure time to shutter control
 *
 *  @Input      ::
 *      pChan:           Vin ID and sensor ID
 *      ExposureTime:   Exposure time / shutter speed
 *
 *  @Output     ::
 *      pShutterCtrl:   Electronic shutter control
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    UINT32 j;

    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* None HDR mode */
        pModeInfo = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            ConvertShutterSpeed(pModeInfo, 0U, pDesiredExposureTime[0], &pActualExposureTime[0], &pShutterCtrl[0]);
        } else {
            /*HDR mode */
            for (j = 0U; j < (pModeInfo->HdrInfo.ActiveChannels); j ++) {
                if(j == 3U) {
                    ConvertShutterSpeed(pModeInfo, j, pDesiredExposureTime[0]/(36.0f), &pActualExposureTime[j], &pShutterCtrl[j]);

                } else {
                    if(j < 4U){
                        ConvertShutterSpeed(pModeInfo, j, pDesiredExposureTime[j], &pActualExposureTime[j], &pShutterCtrl[j]);
                    }
                }
            }
            pActualExposureTime[1] = pActualExposureTime[0];
            pShutterCtrl[1] = pShutterCtrl[0];

            pActualExposureTime[2] = pActualExposureTime[0];
            pShutterCtrl[2] = pShutterCtrl[0];
        }
    }
    //AmbaPrint_PrintUInt5("--- MX00_OX03C_ConvertShutterSpeed, HdrType : %d ---", pModeInfo->HdrInfo.HdrType, 0U, 0U, 0U, 0U);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_SetAnalogGainCtrl
 *
 *  @Description:: Set analog gain control
 *
 *  @Input      ::
 *      pChan:               Vin ID and sensor ID
 *      AnalogGainCtrl:     Analog gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 i,j;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    (void)pModeInfo;
    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* None-HDR mode */
        pModeInfo = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            //GainType = 1;//LCG, DecideLinearGainType();//Needs to Add
            //RetVal |= MX00_OX03C_SetAGainReg(pChan, GainType, pAnalogGainCtrl[0]);
            /* Update current analog gain control */
            for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    MX00_OX03CCtrl[pChan->VinID].CurrentAGCtrl[i][0] = pAnalogGainCtrl[0];
                }
            }
        } else { //HDR mode
            RetVal |= MX00_OX03C_SetAGainReg(pChan, pAnalogGainCtrl);
            //GainType = 1U; //HCG
            //RetVal |= MX00_OX03C_SetAGainReg(pChan, GainType, pAnalogGainCtrl[0]);
            //GainType = 0U; //LCG
            //RetVal |= MX00_OX03C_SetAGainReg(pChan, GainType, pAnalogGainCtrl[1]);
            for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i++) {
                /*if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {*/
                for (j=0U; j<(pModeInfo->HdrInfo.ActiveChannels); j ++) {
                    MX00_OX03CCtrl[pChan->VinID].CurrentAGCtrl[i][j] = pAnalogGainCtrl[j];
                }/*}*/
            }/* HDR mode */
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_IMX224_SetShutterCtrl
 *
 *  @Description:: Set shutter control
 *
 *  @Input      ::
 *      pChan:           Vin ID and sensor ID
 *      ShutterCtrl:    Electronic shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 i,j;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    /////UINT16 SFData, Data ;
    if ((pChan == NULL) || (pShutterCtrl == NULL)) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* None-HDR mode */
        pModeInfo = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_OX03C_SetShutterReg(pChan, pShutterCtrl[0],0);

            for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    MX00_OX03CCtrl[pChan->VinID].CurrentShrCtrl[i][0] = pShutterCtrl[0];
                }
            }
        } else {
            /* HDR mode */
            //Need to check this API, 20190613
            RetVal |= MX00_OX03C_SetShutterReg(pChan, pShutterCtrl[0],0);
            RetVal |= MX00_OX03C_SetShutterReg(pChan, pShutterCtrl[2],2);
            RetVal |= MX00_OX03C_SetShutterReg(pChan, pShutterCtrl[3],3);
            for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    for (j = 0U; j < (pModeInfo->HdrInfo.ActiveChannels); j ++) {
                        MX00_OX03CCtrl[pChan->VinID].CurrentShrCtrl[i][j] = pShutterCtrl[j];
                    }
                }
            }


        }
    }
    //AmbaPrint_PrintUInt5("--- MX00_OX03C_SetShutterCtrl, HdrType : %d ---", pModeInfo->HdrInfo.HdrType, 0U, 0U, 0U, 0U);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_SetDigitalGainCtrl
 *
 *  @Description:: Set digital gain control
 *
 *  @Input      ::
 *      pChan:               Vin ID and sensor ID
 *      DigitalGainCtrl:     Digital gain control for achievable gain factor
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 i,j;
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
    UINT32 RetVal = SENSOR_ERR_NONE;
    //UINT32 GainType = 0; //LCG : 0, HCG : 1

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        /* None-HDR mode */
        pModeInfo = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            //GainType = 0;//Need to add, HCG/LCG
            //RetVal = MX00_OX03C_SetDGainReg(pChan, GainType, pDigitalGainCtrl[0]);
            /* Update current analog gain control */
            for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    MX00_OX03CCtrl[pChan->VinID].CurrentDGCtrl[i][0] = pDigitalGainCtrl[0];
                }
            }
        } else {
            //GainType = 1U;//HCG
            //RetVal = MX00_OX03C_SetDGainReg(pChan, GainType, pDigitalGainCtrl[0]);
            //GainType = 0;//LCG
            //RetVal |= MX00_OX03C_SetDGainReg(pChan, GainType, pDigitalGainCtrl[1]);
            for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    for (j=0U; j<(pModeInfo->HdrInfo.ActiveChannels); j ++) {
                        MX00_OX03CCtrl[pChan->VinID].CurrentDGCtrl[i][j] = pDigitalGainCtrl[j];
                    }
                }
            }/* HDR mode */
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_SetWbGainCtrl
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
static UINT32 MX00_OX03C_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;//CurrentWbCtrl
    const AMBA_SENSOR_MODE_INFO_s *pModeInfo;// = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
    UINT32 i,j;

    if (pChan == NULL) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        pModeInfo = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
        if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_NONE) {
            RetVal = MX00_OX03C_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    MX00_OX03CCtrl[pChan->VinID].CurrentWbCtrl[i][0] = pWbGainCtrl[0];
                }
            }

        } else if (pModeInfo->HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
            RetVal |= MX00_OX03C_SetWbGainReg(pChan, pWbGainCtrl);

            /* Update current wb gain control */
            for (i = 0U; i < MX00_OX03C_NUM_MAX_SENSOR_COUNT; i++) {
                if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0x0U) {
                    for (j=0U; j<(pModeInfo->HdrInfo.ActiveChannels); j ++) {
                        /* Update current shutter control */
                        MX00_OX03CCtrl[pChan->VinID].CurrentWbCtrl[i][j] = pWbGainCtrl[0];
                    }
                }
            }
        } else {
            RetVal = SENSOR_ERR_INVALID_API;
        }

        //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "WWWbgain-set: R=%d Gr=%d Gb=%d B=%d", pWbGainCtrl[0].R*1000, pWbGainCtrl[0].Gr*1000, pWbGainCtrl[0].Gb*1000, pWbGainCtrl[0].B*1000, 0);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_SetSlowShutterCtrl
 *
 *  @Description:: set slow shutter control
 *
 *  @Input      ::
 *      pChan:             Vin ID and sensor ID
 *      SlowShutterCtrl:  Electronic slow shutter control
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_SetSlowShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    (void)pChan;
    (void)SlowShutterCtrl;

    return SENSOR_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX00_OX03C_Config
 *
 *  @Description:: Set sensor to indicated mode
 *
 *  @Input      ::
 *      pChan:             Vin ID and sensor ID
 *      Mode:             Sensor Readout Mode Number
 *      ElecShutterMode:  Operating Mode of the Electronic Shutter
 *
 *  @Output     ::
 *      pModeInfo:  pointer to mode info of target mode
 *
 *  @Return     ::
 *          INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MX00_OX03C_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    const UINT32 EnabledPin[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0]  = 0x0fU,  /* DN/P_DATA_0~3 */
        [AMBA_VIN_CHANNEL1]  = 0xf0U,  /* DN/P_DATA_0~3 */
        /*[AMBA_VIN_CHANNEL2]  = 0x0fU,   DN/P_DATA_0~3 */
        /*[AMBA_VIN_CHANNEL3]  = 0x0fU,   DN/P_DATA_0~3 */
        /*[AMBA_VIN_CHANNEL4]  = 0xf0U,   DN/P_DATA_4~7 */
        /*[AMBA_VIN_CHANNEL5]  = 0xf0U,   DN/P_DATA_4~7 */
        /*[AMBA_VIN_CHANNEL6]  = 0xf0U,   DN/P_DATA_4~7 */
        /*[AMBA_VIN_CHANNEL7]  = 0xf0U,   DN/P_DATA_4~7 */
        /*[AMBA_VIN_CHANNEL8]  = 0x0fU,   SD_LVDS_0~3 */
        /*[AMBA_VIN_CHANNEL9]  = 0x0fU,   SD_LVDS_0~3 */
        /*[AMBA_VIN_CHANNEL10] = 0x0fU,   SD_LVDS_0~3 */
        /*[AMBA_VIN_CHANNEL11] = 0xf0U,   SD_LVDS_4~7 */
        /*[AMBA_VIN_CHANNEL12] = 0xf0U,   SD_LVDS_4~7 */
        /*[AMBA_VIN_CHANNEL13] = 0xf0U,   SD_LVDS_4~7 */
    };

    AMBA_VIN_MIPI_PAD_CONFIG_s MX00_OX03CPadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        //.EnabledPin = EnabledPin[pChan->VinID]
    };

    AMBA_SENSOR_MODE_INFO_s *pModeInfo = &MX00_OX03CCtrl[pChan->VinID].Status.ModeInfo;
    UINT16 SensorMode = (UINT16)pMode->ModeID;
    //UINT8 tmp1,tmp2;
    UINT32 U32RVal = 0;

    MX00_OX03C_FRAME_TIMING_s *pFrameTime = &MX00_OX03CCtrl[pChan->VinID].FrameTime;
    UINT32 RetVal = SENSOR_ERR_NONE;
#ifdef MX00_OX03C_IN_SLAVE_MODE
    UINT32 result;
    const MX00_OX03C_SENSOR_INFO_s *pSensorInfo = &MX00_OX03C_SensorInfo[SensorMode];
#endif
    //static UINT8 Init = 0;
#ifdef MX00_OX03C_SERDES_ENABLE
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Add delay time to wait MAX9296/POC power/PWDN ready to support new MAX9296 with power control by GPIO", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "The delay time bases on HW team's measurement", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power off", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(1);

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Power on", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesPowerCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(3);
#endif
    if (SensorMode >= MX00_OX03C_NUM_MODE) {
        RetVal = SENSOR_ERR_ARG;
    } else {
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "============  [ MX00_OX03C Config ]  ============", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============ init mode:%d ============", SensorMode, 0U, 0U, 0U, 0U);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "======= VinID:%d SensorID:0x%x =======", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);

        MX00_OX03CPadConfig.EnabledPin = EnabledPin[pChan->VinID];

        /* update status */
        MX00_OX03C_PrepareModeInfo(pChan, pMode, pModeInfo);
        U32RVal |= AmbaWrap_memcpy(pFrameTime, &MX00_OX03C_ModeInfoList[SensorMode].FrameTime, sizeof(MX00_OX03C_FRAME_TIMING_s));


#ifdef MX00_OX03C_IN_SLAVE_MODE
        (void)AmbaVIN_MasterSyncDisable(MX00_OX03CMasterSyncChannel[pChan->VinID]);
#endif

        /* Adjust mipi-phy parameters */
        MX00_OX03CPadConfig.DateRate = pModeInfo->OutputInfo.DataRate;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  data rate: %d  ============", (UINT32)MX00_OX03CPadConfig.DateRate, 0U, 0U, 0U, 0U);
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &MX00_OX03CPadConfig);
#ifdef MX00_OX03C_SERDES_ENABLE
        /* After reset VIN, set MAX9296A PWDNB high */
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB low", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(3);
        AmbaPrint_ModulePrintStr5(SENSOR_MODULE_ID, "Set MAX9296A PWDNB high", NULL, NULL, NULL, NULL, NULL);
        RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        RetVal |= Max9295_9296_Init(pChan->VinID, MX00_OX03C_GetEnabledLinkID(pChan->SensorID));

        MX00_OX03C_ConfigSerDes(pChan, pMode->ModeID, pModeInfo);


        /* set MAX9295 MFP2 to output sensor clock */
        if ((pChan->SensorID & MX00_OX03C_SENSOR_ID_CHAN_A) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_A_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
            RetVal |= Max9295_9296_RegisterWrite(pChan->VinID, MX00_MAX9295_A_ID, 0x0100, 0x68); //set max9295 BPP as auto

        }
        if ((pChan->SensorID & MX00_OX03C_SENSOR_ID_CHAN_B) != 0U) {
            (void)Max9295_9296_SetSensorClk(pChan->VinID, MX00_MAX9295_B_ID, MAX9295_9296_RCLK_OUT_FREQ_24M);
            RetVal |= Max9295_9296_RegisterWrite(pChan->VinID, MX00_MAX9295_B_ID, 0x0100, 0x68); //set max9295 BPP as auto
        }
#else
        RetVal |= AmbaUserVIN_SensorClkCtrl(pChan->VinID, pModeInfo->InputClk);
#endif
        RetVal |= MX00_OX03C_HardwareReset(pChan); // sensor hardware reset
        RetVal |= MX00_OX03C_SensorModeInit(pChan, SensorMode);
        SMode = SensorMode;
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  SensorMode: %d  ============", SensorMode, 0U, 0U, 0U, 0U);
#ifdef MX00_OX03C_IN_SLAVE_MODE
        //RetVal |= MX00_OX03C_RegRead(pChan,0x380E,&tmp1);
        //RetVal |= MX00_OX03C_RegRead(pChan,0x380F,&tmp2);
        //RetVal |= MX00_OX03C_RegRead(pChan,0x3882,&tmp3);
        //RetVal |= MX00_OX03C_RegRead(pChan,0x3883,&tmp4);
        result = pSensorInfo->FrameLengthLines - 8U;
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3822, 0x24);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3823, 0x50);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3826, (UINT16)(result & 0x0000FF00U)); //0x05
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3827, (UINT16)(result & 0x000000FFU)); //0x54
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x383E, 0x81);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3881, 0x04);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3882, 0x00);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3883, 0x08);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3836, 0x00);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3837, 0x02);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3009, 0x02);
        RetVal |= MX00_OX03C_RegWrite(pChan, 0x3015, 0x02);
        RetVal |= MX00_OX03C_ConfigMasterSync(pChan, SensorMode);
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "Config Master Sync", 0U, 0U, 0U, 0U, 0U);

#endif
        RetVal |= MX00_OX03C_StandbyOff(pChan); /* Stream on */
        (void)AmbaKAL_TaskSleep(22); //TBD: To avoid i2c fail for gain setting

        RetVal |= MX00_OX03C_ConfigVin(pChan, pModeInfo);
    }

    if(U32RVal != 0U) {
        /* MisraC */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_SENSOR_OBJ_s AmbaSensor_MX00_OX03CObj = {
    .SensorName             = "MX00_OX03C",
    .SerdesName             = "MAX9295_9296",
    .Init                   = MX00_OX03C_Init,
    .Enable                 = MX00_OX03C_Enable,
    .Disable                = MX00_OX03C_Disable,
    .Config                 = MX00_OX03C_Config,

    .GetStatus              = MX00_OX03C_GetStatus,
    .GetModeInfo            = MX00_OX03C_GetModeInfo,
    .GetDeviceInfo          = MX00_OX03C_GetDeviceInfo,
    .GetHdrInfo             = MX00_OX03C_GetHdrInfo,
    .GetCurrentGainFactor   = MX00_OX03C_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX00_OX03C_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX00_OX03C_ConvertGainFactor,
    .ConvertShutterSpeed    = MX00_OX03C_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX00_OX03C_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = MX00_OX03C_SetDigitalGainCtrl, //Need to add
    .SetWbGainCtrl          = MX00_OX03C_SetWbGainCtrl,      //Need to add
    .SetShutterCtrl         = MX00_OX03C_SetShutterCtrl,
    .SetSlowShutterCtrl     = MX00_OX03C_SetSlowShutterCtrl, //Skipped it first

    .RegisterRead           = MX00_OX03C_RegisterRead,
    .RegisterWrite          = MX00_OX03C_RegisterWrite,

    .ConfigPost             = NULL,
    .GetSerdesStatus    = NULL,

    .ConvertStrobeWidth     = NULL,
    .SetStrobeCtrl          = NULL,
};
