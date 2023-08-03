/**
 *  @file AmbaYuv_MAX9295_96712_AmbaVout.c
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
 *  @details Control APIs of MAX9295_96712_AMBAVOUT YUV422 with MIPI interface
 *
 */

#include "AmbaKAL.h"
#include "AmbaWrap.h"

#include "AmbaVIN.h"
#include "AmbaGPIO.h"
#include "AmbaYuv.h"
#include "AmbaYuv_MAX9295_96712_AmbaVout.h"
#ifdef MAXIM_setting
#include "AmbaSbrg_Max9295_96712.h"
#endif
#include "AmbaPrint.h"

//#include "bsp.h"

#define AMBA_MX01_VOUT_MAX_VIN_CHANNEL        (4U)

//#define MAXIM_setting
/*-----------------------------------------------------------------------------------------------*\
 * AMBA_MX01_VOUT runtime status
\*-----------------------------------------------------------------------------------------------*/
static AMBA_YUV_STATUS_INFO_s AMBA_MX01_VOUTStatus[AMBA_MX01_VOUT_MAX_VIN_CHANNEL] = {0};

#ifdef MAXIM_setting
static MAX9295_96712_SERDES_CONFIG2_s MX01_AmbaVoutSerdesConfig = {
    .EnabledLinkID   = 0x03U,
    .SensorSlaveID   =  {
        [0] = 0x42,
        [1] = 0x42,
        [2] = 0x42,
        [3] = 0x42,
    },
    .SensorBCID      = 0x44,
    .SensorAliasID   = {
        [0] = 0x60U,
        [1] = 0x62U,
        [2] = 0x64U,
        [3] = 0x66U,
    },
    .DataType        = {
        [0] = 0x1eU,  /* Default DataType: YUV424 */
        [1] = 0x2cU,  /* Default DataType: RAW12 */
        [2] = 0x2cU,  /* Default DataType: RAW12 */
        [3] = 0x2cU,  /* Default DataType: RAW12 */
    },
    .DataType2       = {
        [0] = 0xffU,  /* Default secondary DataType: not used */
        [1] = 0xffU,  /* Default secondary DataType: not used */
        [2] = 0xffU,  /* Default secondary DataType: not used */
        [3] = 0xffU,  /* Default secondary DataType: not used */
    },
    .CSIRxLaneNum    = {2, 4, 4, 4},
    .CSITxLaneNum    = {4, 4, 4, 4},

    .CSITxSpeed      = {10, 10, 10, 10},
    .VideoAggregationMode = MAX9295_MAX96712_VIDEO_AGGR_FCFS,
};
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AMBA_MX01_VOUT_HardwareReset
 *
 *  @Description:: Reset AMBA_MX01_VOUT Yuv Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AMBA_MX01_VOUT_HardwareReset(void)
{
    UINT32 RetVal = YUV_ERR_NONE;
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_113, AMBA_GPIO_LEVEL_LOW);
    (void)AmbaKAL_TaskSleep(2);
    RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_113, AMBA_GPIO_LEVEL_HIGH);
    (void)AmbaKAL_TaskSleep(2);
    return RetVal;
}
#ifdef MAXIM_setting
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MX01_VOUT_ConfigSerDes
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
static void MX01_VOUT_ConfigSerDes(const AMBA_YUV_CHANNEL_s *pChan, const AMBA_MX01_VOUT_MODE_INFO_s *pModeInfo)
{
    const AMBA_MX01_VOUT_MODE_INFO_s  *pvoutInfo = pModeInfo;
    MAX9295_96712_SERDES_CONFIG2_s *pSerdesCfg = &MX01_AmbaVoutSerdesConfig;
    DOUBLE FloorVal;

    pSerdesCfg->DataType[0] = 0x1eU;  /* YUV422 */
    pSerdesCfg->NumDataBits[0] = pvoutInfo->NumDataBits;
    pSerdesCfg->CSIRxLaneNum[0] = 2; //pvoutInfo->NumDataLanes;
    pSerdesCfg->CSITxLaneNum[0] = 4; //pvoutInfo->NumDataLanes;

    pSerdesCfg->DataType[1] = 0x2c;  /* YUV422 */
    pSerdesCfg->NumDataBits[1] = 12;
    pSerdesCfg->CSIRxLaneNum[1] = 4; //pvoutInfo->NumDataLanes;
    pSerdesCfg->CSITxLaneNum[1] = 4; //pvoutInfo->NumDataLanes;

    (void) AmbaWrap_floor(900000000 * 1e-8, &FloorVal);
    pSerdesCfg->CSITxSpeed[0] = (UINT8)FloorVal;

    (void) Max9295_96712_Config2(pChan->VinID, &MX01_AmbaVoutSerdesConfig);
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AMBA_MX01_VOUT_ConfigVin
 *
 *  @Description:: Configure VIN to receieve output data of the new mode
 *
 *  @Input      ::
 *      pModeInfo:  Details of the specified Yuv mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AMBA_MX01_VOUT_ConfigVin(UINT32 VinID, const AMBA_MX01_VOUT_MODE_INFO_s *pModeInfo)
{
    static AMBA_VIN_MIPI_CONFIG_s AMBA_MX01_VOUTVinConfig = {
        .Config             = {
            .FrameRate          = {
                .Interlace          = 0,
                .TimeScale          = 0,
                .NumUnitsInTick     = 0,
            },
            .ColorSpace         = AMBA_VIN_COLOR_SPACE_YUV,
            .YuvOrder           = AMBA_VIN_YUV_ORDER_CB_Y0_CR_Y1,
            .NumDataBits        = 10U,
            .NumSkipFrame       = 1U,
            .RxHvSyncCtrl       = {
                .NumActivePixels    = 0,
                .NumActiveLines     = 0,
                .NumTotalPixels     = 0,
                .NumTotalLines      = 0,
            },
            .SplitCtrl          = {0},
            .DelayedVsync       = 0,
        },
        .NumActiveLanes     = 4U,
        .DataType           = 0x00U,
        .DataTypeMask       = 0x3f,
//#if defined(CONFIG_SENSOR_VCHDR_SUPPORT)
        .VirtChanHDREnable  = 0U,
        .VirtChanHDRConfig  = {0}
//#endif
    };

    UINT32 RetVal = YUV_ERR_NONE;
    if (pModeInfo == NULL) {
        RetVal = YUV_ERR_ARG;
    } else {
        AMBA_VIN_MIPI_CONFIG_s VinCfg;

        (void)AmbaWrap_memset(&VinCfg, 0x0, sizeof(AMBA_VIN_MIPI_CONFIG_s));
        (void)AmbaWrap_memcpy(&VinCfg, &AMBA_MX01_VOUTVinConfig, sizeof(AMBA_VIN_MIPI_CONFIG_s));
        VinCfg.NumActiveLanes = pModeInfo->NumDataLanes;
        VinCfg.Config.NumDataBits = pModeInfo->NumDataBits;

        VinCfg.Config.RxHvSyncCtrl.NumTotalPixels  = pModeInfo->OutputInfo.OutputWidth;
        VinCfg.Config.RxHvSyncCtrl.NumActivePixels  = pModeInfo->OutputInfo.OutputWidth;
        VinCfg.Config.RxHvSyncCtrl.NumTotalLines   = pModeInfo->OutputInfo.OutputHeight;
        VinCfg.Config.RxHvSyncCtrl.NumActiveLines   = pModeInfo->OutputInfo.OutputHeight;

        (void)AmbaWrap_memcpy(&VinCfg.Config.FrameRate, &pModeInfo->OutputInfo.FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s));

        RetVal = AmbaVIN_MipiConfig(VinID, &VinCfg);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AMBA_MX01_VOUT_Init
 *
 *  @Description:: Initialize Yuv device driver
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AMBA_MX01_VOUT_Init(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT32 RetVal = YUV_ERR_NONE;
    if (pChan->VinID > AMBA_NUM_VIN_CHANNEL) {
        RetVal = YUV_ERR_ARG;
    }
    AmbaPrint_PrintUInt5("AMBA_MX01_VOUT_Init 2VC", 0U, 0U, 0U, 0U, 0U);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AMBA_MX01_VOUT_Enable
 *
 *  @Description:: Power on Yuv Device
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AMBA_MX01_VOUT_Enable(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT32 RetVal;
    if (pChan == NULL) {
        RetVal = YUV_ERR_ARG;
    } else if (pChan->VinID > AMBA_NUM_VIN_CHANNEL) {
        RetVal = YUV_ERR_ARG;
    } else {
        RetVal = AMBA_MX01_VOUT_HardwareReset();
    }
    AmbaPrint_PrintUInt5("AMBA_MX01_VOUT_Enable 2VC", 0U, 0U, 0U, 0U, 0U);
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AMBA_MX01_VOUT_GetStatus
 *
 *  @Description:: Get current Yuv device status
 *
 *  @Input      ::
 *      pChan:   Vin ID
 *
 *  @Output     ::
 *      pStatus:    pointer to current status
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AMBA_MX01_VOUT_GetStatus(const AMBA_YUV_CHANNEL_s *pChan, AMBA_YUV_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = YUV_ERR_NONE;
    if ((pChan == NULL) || (pChan->VinID > AMBA_NUM_VIN_CHANNEL) || (pStatus == NULL)) {
        RetVal = YUV_ERR_ARG;
    } else {
        (void) AmbaWrap_memcpy(pStatus, &AMBA_MX01_VOUTStatus[pChan->VinID], sizeof(AMBA_YUV_STATUS_INFO_s));
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AMBA_MX01_VOUT_GetModeInfo
 *
 *  @Description:: Get current ModeInfo
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *      Mode:   don't care
 *
 *  @Output     ::
 *      pModeInfo:    pointer to current mode info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AMBA_MX01_VOUT_GetModeInfo(const AMBA_YUV_CHANNEL_s *pChan, const AMBA_YUV_CONFIG_s *pMode, AMBA_YUV_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = YUV_ERR_NONE;

    if ((pChan == NULL) || (pChan->VinID > AMBA_NUM_VIN_CHANNEL) || (pMode == NULL) || (pModeInfo == NULL)) {
        RetVal = YUV_ERR_ARG;
    } else if (pMode->ModeID >= AMBA_MX01_VOUT_MODE) {
        RetVal = YUV_ERR_ARG;
    } else {
        pModeInfo->Config.ModeID = pMode->ModeID;
        (void)AmbaWrap_memcpy(&pModeInfo->OutputInfo, &AmbaVout_MX01_ModeInfo[pMode->ModeID].OutputInfo, sizeof(AMBA_YUV_OUTPUT_INFO_s));
    }

    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AMBA_MX01_VOUT_Config
 *
 *  @Description:: Set Yuv device to indicated mode
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *      Mode:   pointer to mode configuration represented with AMBA_MX01_VOUT_MODE_INFO_s
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 AMBA_MX01_VOUT_Config(const AMBA_YUV_CHANNEL_s *pChan, const AMBA_YUV_CONFIG_s *pMode)
{
    UINT32 RetVal = YUV_ERR_NONE;

    AMBA_VIN_MIPI_VC_CONFIG_s MipiVirtChanConfig = {
        .VirtChan     = 0x0U,
        .VirtChanMask = 0x0U,
    };

    if ((pChan == NULL)  || (pChan->VinID > AMBA_NUM_VIN_CHANNEL) || (pMode == NULL)) {
        RetVal = YUV_ERR_ARG;
    } else if (pMode->ModeID >= AMBA_MX01_VOUT_MODE) {
        RetVal = YUV_ERR_ARG;
    } else {
        UINT32 VinID = pChan->VinID;
        AMBA_MX01_VOUT_MODE_INFO_s ModeInfo = {0};
        AMBA_VIN_MIPI_PAD_CONFIG_s VinMipiCtrl = {
            .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
            .DateRate   = 0U,
            .EnabledPin = 0xfU
        };


        AmbaPrint_PrintUInt5("============  [ Amba Vout MAX9295_96712 2VC] Mode: %d Vin ID: %d============", pMode->ModeID, VinID, 0U, 0U, 0U);

        (void)AmbaWrap_memcpy(&AMBA_MX01_VOUTStatus[VinID].ModeInfo.Config, pMode, sizeof(AMBA_YUV_CONFIG_s));
        (void)AmbaWrap_memcpy(&AMBA_MX01_VOUTStatus[VinID].ModeInfo.OutputInfo, &AmbaVout_MX01_ModeInfo[pMode->ModeID].OutputInfo, sizeof(AMBA_YUV_OUTPUT_INFO_s));
        (void)AmbaWrap_memcpy(&ModeInfo, &AmbaVout_MX01_ModeInfo[pMode->ModeID], sizeof(AMBA_MX01_VOUT_MODE_INFO_s));

        AmbaPrint_PrintUInt5("============ init MAX9295_96712 Yuv mode:%dx%d Rate %u Lanes %d 2VC ============",
                             ModeInfo.OutputInfo.RecordingPixels.Width,
                             ModeInfo.OutputInfo.RecordingPixels.Height,
                             900000000U,
                             ModeInfo.NumDataLanes, 0U);

        VinMipiCtrl.DateRate = 900000000U;

        RetVal = AmbaVIN_MipiReset(VinID, &VinMipiCtrl);

#ifdef MAXIM_setting
        //RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_087, AMBA_GPIO_LEVEL_LOW);
        (void)AmbaKAL_TaskSleep(3);
        AmbaPrint_PrintUInt5("PWDN HIGH", 0U, 0U, 0U, 0U, 0U);
        //RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
        RetVal |= AmbaGPIO_SetFuncGPO(GPIO_PIN_087, AMBA_GPIO_LEVEL_HIGH);
        (void)AmbaKAL_TaskSleep(3);

        RetVal |= Max9295_96712_Init(pChan->VinID,0x3U);

        MX01_VOUT_ConfigSerDes(pChan, &ModeInfo);
        //send sensor clk
        RetVal |= Max9295_96712_SetSensorClk(pChan->VinID, MAX9295_B_ID, MAX9295_96712_RCLK_OUT_FREQ_26973027); //MAX9295_96712_RCLK_OUT_FREQ_26973027, MAX9295_96712_RCLK_OUT_FREQ_27M
        //reset sensor
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MAX9295_B_ID, MAX9295_96712_GPIO_PIN_4, 0);
        (void) AmbaKAL_TaskSleep(1);  /* XCLR Low level pulse width >= 500ns */
        RetVal |= Max9295_96712_SetGpioOutput(pChan->VinID, MAX9295_B_ID, MAX9295_96712_GPIO_PIN_4, 1);
#endif

        (void)AmbaKAL_TaskSleep(3);


        if (RetVal == YUV_ERR_NONE) {
            RetVal |= AMBA_MX01_VOUT_ConfigVin(AMBA_VIN_CHANNEL0, &ModeInfo);
            MipiVirtChanConfig.VirtChan = 0x0U;
            RetVal |= AmbaVIN_MipiVirtChanConfig(AMBA_VIN_CHANNEL0, &MipiVirtChanConfig);
        }

    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_YUV_OBJ_s AmbaYuv_MX01_AmbaVoutObj = {
    .Init           = AMBA_MX01_VOUT_Init,
    .Enable         = AMBA_MX01_VOUT_Enable,
    .Disable        = NULL,          /* AMBA_MX01_VOUT does not support SW standby mode. */
    .Config         = AMBA_MX01_VOUT_Config,
    .GetStatus      = AMBA_MX01_VOUT_GetStatus,
    .GetModeInfo    = AMBA_MX01_VOUT_GetModeInfo
};
