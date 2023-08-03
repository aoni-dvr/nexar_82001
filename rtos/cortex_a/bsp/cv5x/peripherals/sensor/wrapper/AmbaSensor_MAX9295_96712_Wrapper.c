/**
 *  @file AmbaSensor_MAX9295_96712_Wrapper.c
 *
 *  Copyright (c) 2022 Ambarella International LP
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
 *  @details Control APIs of MAX9295_96712 driver wrapper
 *
 */

#include <AmbaWrap.h>
#include "AmbaMisraFix.h"

#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaVIN.h"
#include "AmbaGPIO.h"
#include "AmbaSensor.h"
#include "AmbaSensor_MAX9295_96712_IMX424_RCCB.h"
#include "AmbaSensor_MAX9295_96712_IMX390.h"
#include "AmbaSensor_MAX9295_96712_Wrapper.h"
#include "AmbaSbrg_Max9295_96712.h"

#include "bsp.h"

#define MX01_WRAPPER_NUM_CTRL_BUF    4

extern MX01_SENSOR_OBJ_s MX01_IMX424_Obj;
extern MX01_SENSOR_OBJ_s MX01_IMX390_Obj;

typedef struct {
    MX01_SENSOR_OBJ_s              *pDriver[MAX96712_NUM_RX_PORT];
    MAX9295_96712_SERDES_CONFIG2_s SerdesConfig;
} MX01_WRAPPER_CTRL_s;

static MX01_WRAPPER_CTRL_s MX01_WrapperCtrlBuf[MX01_WRAPPER_NUM_CTRL_BUF] = {
    [0 ... (MX01_WRAPPER_NUM_CTRL_BUF - 1)] = {
        .pDriver = {
            [0] = &MX01_IMX424_Obj,
            [1] = &MX01_IMX424_Obj,
            [2] = &MX01_IMX424_Obj,
            [3] = &MX01_IMX424_Obj,
        },
        .SerdesConfig = {
            .EnabledLinkID   = 0x01U,
            .SensorSlaveID   =  {
                [0] = 0x00U,
                [1] = 0x00U,
                [2] = 0x00U,
                [3] = 0x00U,
            },
            .SensorBCID      = MX01_IMX424_I2C_BC_SLAVE_ADDRESS,
            .SensorAliasID   = {
                [0] = 0x60U,
                [1] = 0x62U,
                [2] = 0x64U,
                [3] = 0x66U,
            },
            .DataType        = {
                [0] = 0x2cU,  /* Default DataType: RAW12 */
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
            .CSIRxLaneNum    = {4, 4, 4, 4},
            .CSITxLaneNum    = {4, 4, 4, 4},

            .CSITxSpeed      = {10, 10, 10, 10},

            .VideoAggregationMode = MAX9295_MAX96712_VIDEO_AGGR_FCFS,
        },
    },
};

static MX01_WRAPPER_CTRL_s *pMX01_WrapperCtrl[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0  ... AMBA_VIN_CHANNEL3]  = &MX01_WrapperCtrlBuf[0],
    [AMBA_VIN_CHANNEL4  ... AMBA_VIN_CHANNEL7]  = &MX01_WrapperCtrlBuf[1],
    [AMBA_VIN_CHANNEL8  ... AMBA_VIN_CHANNEL10] = &MX01_WrapperCtrlBuf[2],
    [AMBA_VIN_CHANNEL11 ... AMBA_VIN_CHANNEL13] = &MX01_WrapperCtrlBuf[3],
};

static UINT32 MX01_WrapperMasterSyncChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0]  = AMBA_SENSOR_MSYNC_CHANNEL,
    [AMBA_VIN_CHANNEL1]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN1,
    [AMBA_VIN_CHANNEL2]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN2,
    [AMBA_VIN_CHANNEL3]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN3,
    [AMBA_VIN_CHANNEL4]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN4,
    [AMBA_VIN_CHANNEL5]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN5,
    [AMBA_VIN_CHANNEL6]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN6,
    [AMBA_VIN_CHANNEL7]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN7,
    [AMBA_VIN_CHANNEL8]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN8,
    [AMBA_VIN_CHANNEL9]  = AMBA_SENSOR_MSYNC_CHANNEL_VIN9,
    [AMBA_VIN_CHANNEL10] = AMBA_SENSOR_MSYNC_CHANNEL_VIN10,
    [AMBA_VIN_CHANNEL11] = AMBA_SENSOR_MSYNC_CHANNEL_VIN11,
    [AMBA_VIN_CHANNEL12] = AMBA_SENSOR_MSYNC_CHANNEL_VIN12,
    [AMBA_VIN_CHANNEL13] = AMBA_SENSOR_MSYNC_CHANNEL_VIN13,
};

static UINT32 MX01_Wrapper_ConfigMasterSync(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 ModeID, UINT32 SlowShutterCtrl)
{
    UINT32 RetVal = SENSOR_ERR_NONE;
    DOUBLE PeriodInDb;
    AMBA_VIN_FRAME_RATE_s FrameRate = {0};
    AMBA_VIN_MASTER_SYNC_CONFIG_s MasterSyncCfg = {
        .RefClk = 24000000,
        .HSync = {
            .Period     = 0,
            .PulseWidth = 8,
            .Polarity   = 0
        },
        .VSync = {
            .Period     = 1,
            .PulseWidth = 1000,
            .Polarity   = 0
        },
        .HSyncDelayCycles = 0,
        .VSyncDelayCycles = 0,
        .ToggleHsyncInVblank = 1
    };

    (void) pMX01_WrapperCtrl[pChan->VinID]->pDriver[0]->GetFrameRateInfo(ModeID, &FrameRate); /* TBD */

    if (AmbaWrap_floor(((DOUBLE)FrameRate.NumUnitsInTick * ((DOUBLE) MasterSyncCfg.RefClk / (DOUBLE)FrameRate.TimeScale)) + 0.5, &PeriodInDb) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    } else {
        MasterSyncCfg.HSync.Period = (UINT32)PeriodInDb;
        MasterSyncCfg.VSync.Period = SlowShutterCtrl;

        RetVal = AmbaVIN_MasterSyncEnable(MX01_WrapperMasterSyncChannel[pChan->VinID], &MasterSyncCfg);
    }

    return RetVal;
}

#if 0
static UINT32 MX01_Wrapper_SetMasterSyncCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    return MX01_Wrapper_ConfigMasterSync(pChan, pMX01_IMX390Ctrl[pChan->VinID]->Status[pChan->VinID].ModeInfo.Config.ModeID, SlowShutterCtrl);
}
#endif

static UINT32 MX01_Wrapper_GetModeID(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 ModeID = pMode->ModeID;

    if ((pChan->SensorID & MX01_WRAPPER_SENSOR_ID_CHAN_A) != 0U) {
        ModeID = pMode->ModeID;
    } else if ((pChan->SensorID & MX01_WRAPPER_SENSOR_ID_CHAN_B) != 0U) {
        ModeID = pMode->ModeID_1;
    } else if ((pChan->SensorID & MX01_WRAPPER_SENSOR_ID_CHAN_C) != 0U) {
        ModeID = pMode->ModeID_2;
    } else if ((pChan->SensorID & MX01_WRAPPER_SENSOR_ID_CHAN_D) != 0U) {
        ModeID = pMode->ModeID_3;
    } else {
        /* avoid misraC error */
    }

    return ModeID;
}

static UINT32 MX01_Wrapper_UpdateSerdesCSITxConfig(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 i, ModeID;
    UINT32 DataRate;
    UINT64 WorkUINT64, TotalBandwidth = 0U;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo = NULL;
    MAX9295_96712_SERDES_CONFIG2_s *pSerdesCfg = NULL;
    DOUBLE CeilVal;
    UINT8 CSITxLaneNum = 1U;
    AMBA_SENSOR_CHANNEL_s TmpChan;
    UINT32 RetVal = SENSOR_ERR_NONE;
    ULONG Addr;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            TmpChan.VinID = pChan->VinID;
            TmpChan.SensorID = (UINT32)1U << (4U * (i + 1U));

            ModeID = MX01_Wrapper_GetModeID(&TmpChan, pMode);

            pOutputInfo = &pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pOutputInfo[ModeID];

            WorkUINT64 = (UINT64) pOutputInfo->DataRate;
            WorkUINT64 *= (UINT64) pOutputInfo->NumDataLanes;

            TotalBandwidth += WorkUINT64;

            if (CSITxLaneNum < pOutputInfo->NumDataLanes) {
                CSITxLaneNum = pOutputInfo->NumDataLanes;
            }
            (void)pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->GetSerdesCfgAddr(&TmpChan, &Addr);
            AmbaMisra_TypeCast(&pSerdesCfg, &Addr);

            /* update sensor slave ID */
            pMX01_WrapperCtrl[pChan->VinID]->SerdesConfig.SensorSlaveID[i] = pSerdesCfg->SensorSlaveID[i];
            pMX01_WrapperCtrl[pChan->VinID]->SerdesConfig.SensorBCID= pSerdesCfg->SensorBCID; /* TBD */
        }
    }

    DataRate = (UINT32) (TotalBandwidth / (UINT64) CSITxLaneNum);

    if (DataRate > 2500000000U) {
        AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "DataRate %d bps/lane over Maxim 2.5Gbps/lane limitation, set 2.5Gbps/lane instead", DataRate, 0U, 0U, 0U, 0U);
        DataRate = 2500000000U;
    }

    if (AmbaWrap_ceil((DOUBLE)DataRate * 1e-8, &CeilVal) != ERR_NONE) {
        RetVal = SENSOR_ERR_UNEXPECTED;
    }
    /* update TX speed configuration */
    pMX01_WrapperCtrl[pChan->VinID]->SerdesConfig.CSITxSpeed[0] = (UINT8)CeilVal;
    pMX01_WrapperCtrl[pChan->VinID]->SerdesConfig.CSITxLaneNum[0] = CSITxLaneNum;

    /* write back TX speed configuration to each sensor driver */
    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            TmpChan.VinID = pChan->VinID;
            TmpChan.SensorID = (UINT32)1U << (4U * (i + 1U));

            (void)pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->GetSerdesCfgAddr(&TmpChan, &Addr);
            AmbaMisra_TypeCast(&pSerdesCfg, &Addr);

            pSerdesCfg->CSITxSpeed[0] = (UINT8)CeilVal;
            pSerdesCfg->CSITxLaneNum[0] = CSITxLaneNum;
        }
    }

    return RetVal;
}

static UINT16 MX01_Wrapper_GetEnabledLinkID(UINT32 SensorID)
{
    UINT32 EnabledLinkID = 0U, i;

    for (i = 0U; i < MX01_WRAPPER_NUM_MAX_SENSOR_COUNT; i ++) {
        if ((SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            EnabledLinkID = EnabledLinkID | ((UINT32)1U << i);
        }
    }

    return (UINT16)EnabledLinkID;
}

static UINT32 MX01_Wrapper_GetDataType(UINT8 NumDataBits, UINT8 *pDataType)
{
    UINT32 RetVal = SENSOR_ERR_NONE;

    if (NumDataBits == 10U) {
        *pDataType = 0x2bU;
    } else if (NumDataBits == 12U) {
        *pDataType = 0x2cU;
    } else if (NumDataBits == 14U) {
        *pDataType = 0x2dU;
    } else if (NumDataBits == 16U) {
        *pDataType = 0x2eU;
    } else {
        RetVal = SENSOR_ERR_ARG;
    }

    return RetVal;
}

static void MX01_Wrapper_ConfigSerDes(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pModeList)
{
    MX01_WRAPPER_SENSOR_INFO_s Mx01SensorInfo = {0};
    MAX9295_96712_SERDES_CONFIG2_s *pSerdesCfg = &pMX01_WrapperCtrl[pChan->VinID]->SerdesConfig;
    UINT32 i;

    pSerdesCfg->EnabledLinkID = MX01_Wrapper_GetEnabledLinkID(pChan->SensorID);

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            if (pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->GetSensorInfo(pModeList[i], &Mx01SensorInfo) != ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_96712_Wrapper] Can't get sensor info", 0U, 0U, 0U, 0U, 0U);
            }

            if (MX01_Wrapper_GetDataType(Mx01SensorInfo.NumDataBits, &pSerdesCfg->DataType[i]) != SENSOR_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "[MAX9295_96712_Wrapper] Can't get DataType for NumDataBits[%d]=%d", i, Mx01SensorInfo.NumDataBits, 0U, 0U, 0U);
            }
            pSerdesCfg->NumDataBits[i] = Mx01SensorInfo.NumDataBits;
            pSerdesCfg->CSIRxLaneNum[i] = Mx01SensorInfo.NumDataLanes;
        }
    }

    (void) Max9295_96712_Config2(pChan->VinID, &pMX01_WrapperCtrl[pChan->VinID]->SerdesConfig);
}

static UINT32 MX01_Wrapper_Init(const AMBA_SENSOR_CHANNEL_s *pChan)
{
#if 0
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*Init)(const AMBA_SENSOR_CHANNEL_s *pChan) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            Init = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->Init;
            if (Init != NULL) {
                RetVal = Init(pChan);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }
#else /* workaround for SensorID == 0x1 */
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*Init)(const AMBA_SENSOR_CHANNEL_s *pChan) = NULL;
    AMBA_SENSOR_CHANNEL_s TmpChan = *pChan;

    if (TmpChan.SensorID == 0x1) {
        TmpChan.SensorID = 0x10;
    }

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((TmpChan.SensorID & (0x10U << (i << 2U))) != 0U) {
            Init = pMX01_WrapperCtrl[TmpChan.VinID]->pDriver[i]->pSensorObj->Init;
            if (Init != NULL) {
                RetVal = Init(&TmpChan);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }
#endif

    return RetVal;
}

static UINT32 MX01_Wrapper_Enable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
#if 0
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*Enable)(const AMBA_SENSOR_CHANNEL_s *pChan) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            Enable = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->Enable;
            if (Enable != NULL) {
                RetVal = Enable(pChan);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }
#else /* workaround for SensorID == 0x1 */
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*Enable)(const AMBA_SENSOR_CHANNEL_s *pChan) = NULL;
    AMBA_SENSOR_CHANNEL_s TmpChan = *pChan;

    if (TmpChan.SensorID == 0x1) {
        TmpChan.SensorID = 0x10;
    }

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((TmpChan.SensorID & (0x10U << (i << 2U))) != 0U) {
            Enable = pMX01_WrapperCtrl[TmpChan.VinID]->pDriver[i]->pSensorObj->Enable;
            if (Enable != NULL) {
                RetVal = Enable(&TmpChan);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }
#endif

    return RetVal;
}

static UINT32 MX01_Wrapper_Disable(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*Disable)(const AMBA_SENSOR_CHANNEL_s *pChan) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            Disable = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->Disable;
            if (Disable != NULL) {
                RetVal = Disable(pChan);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_Config(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*Config)(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode) = NULL;
    AMBA_SENSOR_CHANNEL_s TmpChan;
    AMBA_VIN_MIPI_PAD_CONFIG_s PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
    };
    AMBA_VIN_MIPI_VC_CONFIG_s MipiVirtChanConfig = {
        .VirtChan     = 0x0U,
        .VirtChanMask = 0x0U,
    };
    const UINT8 LaneShift[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0]  = 0U,
        [AMBA_VIN_CHANNEL1]  = 0U,
        [AMBA_VIN_CHANNEL2]  = 0U,
        [AMBA_VIN_CHANNEL3]  = 0U,
        [AMBA_VIN_CHANNEL4]  = 4U,
        [AMBA_VIN_CHANNEL5]  = 4U,
        [AMBA_VIN_CHANNEL6]  = 4U,
        [AMBA_VIN_CHANNEL7]  = 4U,
        [AMBA_VIN_CHANNEL8]  = 0U,
        [AMBA_VIN_CHANNEL9]  = 0U,
        [AMBA_VIN_CHANNEL10] = 0U,
        [AMBA_VIN_CHANNEL11] = 4U,
        [AMBA_VIN_CHANNEL12] = 4U,
        [AMBA_VIN_CHANNEL13] = 4U,
    };
    AMBA_SENSOR_MODE_INFO_s ModeInfo[MX01_WRAPPER_NUM_MAX_SENSOR_COUNT];
    UINT32 ModeID[MX01_WRAPPER_NUM_MAX_SENSOR_COUNT] = {0xffffffffU, 0xffffffffU, 0xffffffffU, 0xffffffffU};

    ModeID[0] = pMode->ModeID;
    ModeID[1] = pMode->ModeID_1;
    ModeID[2] = pMode->ModeID_2;
    ModeID[3] = pMode->ModeID_3;

    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  [ MAX9295_96712_WRAPPER_Config VC ]  ============", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============ init mode:%d %d %d %d ============", pMode->ModeID, pMode->ModeID_1, pMode->ModeID_2, pMode->ModeID_3, 0U);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "======= VinID:%d SensorID:0x%x =======", pChan->VinID, pChan->SensorID, 0U, 0U, 0U);

    (void) MX01_Wrapper_UpdateSerdesCSITxConfig(pChan, pMode);

    for (i = 0U; i < MX01_WRAPPER_NUM_MAX_SENSOR_COUNT; i++) {
        if ((pChan->SensorID & ((UINT32)1U << (4U * (i + 1U)))) != 0U) {
            TmpChan.VinID = pChan->VinID;
            TmpChan.SensorID = (UINT32)1U << (4U * (i + 1U));

            /* update status */
            RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->GetModeInfo(&TmpChan, pMode, &ModeInfo[i]);
        }
    }

    (void) AmbaVIN_MasterSyncDisable(MX01_WrapperMasterSyncChannel[pChan->VinID]);

    /* Adjust mipi-phy parameters */
    PadConfig.DateRate = (UINT64) pMX01_WrapperCtrl[pChan->VinID]->SerdesConfig.CSITxSpeed[0] * 100000000U;
    PadConfig.EnabledPin = (((UINT32)1U << pMX01_WrapperCtrl[pChan->VinID]->SerdesConfig.CSITxLaneNum[0]) - 1U) << LaneShift[pChan->VinID];
    //AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID, "============  data rate: %d  ============", PadConfig.DateRate, 0U, 0U, 0U, 0U);
    if((pChan->SensorID & MX01_WRAPPER_SENSOR_ID_CHAN_A) != 0U) {
        RetVal |= AmbaVIN_MipiReset(pChan->VinID, &PadConfig);
    }
    if((pChan->SensorID & MX01_WRAPPER_SENSOR_ID_CHAN_B) != 0U) {
        RetVal |= AmbaVIN_MipiReset(pChan->VinID+1U, &PadConfig);
    }
    if((pChan->SensorID & MX01_WRAPPER_SENSOR_ID_CHAN_C) != 0U) {
        RetVal |= AmbaVIN_MipiReset(pChan->VinID+2U, &PadConfig);
    }
    if((pChan->SensorID & MX01_WRAPPER_SENSOR_ID_CHAN_D) != 0U) {
        RetVal |= AmbaVIN_MipiReset(pChan->VinID+3U, &PadConfig);
    }

    /* After reset VIN, Set MAX96712A PWDNB high */
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"PWDN LOW", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_LOW);
    (void) AmbaKAL_TaskSleep(3);
    AmbaPrint_ModulePrintUInt5(SENSOR_MODULE_ID,"PWDN HIGH", 0U, 0U, 0U, 0U, 0U);
    RetVal |= AmbaUserGPIO_SerdesResetCtrl(pChan->VinID, AMBA_GPIO_LEVEL_HIGH);
    (void) AmbaKAL_TaskSleep(3);

    (void) Max9295_96712_Init(pChan->VinID, MX01_Wrapper_GetEnabledLinkID(pChan->SensorID));
    (void) MX01_Wrapper_ConfigSerDes(pChan, ModeID);

    (void) AmbaWrap_memcpy(&TmpChan, pChan, sizeof(TmpChan));
    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            Config = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->Config;
            TmpChan.SensorID = (0x10U << (i << 2U));
            if (Config != NULL) {
                RetVal = Config(&TmpChan, pMode);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    RetVal |= MX01_Wrapper_ConfigMasterSync(pChan, ModeID[0], 1U);

    /* config vin */
    if((pChan->SensorID & MX01_WRAPPER_SENSOR_ID_CHAN_A) != 0U) {
        TmpChan.SensorID = MX01_WRAPPER_SENSOR_ID_CHAN_A;

        RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[0]->ConfigVinPre(&TmpChan, pMode);
        RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[0]->ConfigVin(pChan->VinID, &ModeInfo[0]);
        MipiVirtChanConfig.VirtChan = 0x0U;
        RetVal |= AmbaVIN_MipiVirtChanConfig(pChan->VinID, &MipiVirtChanConfig);

        RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[0]->ConfigVinPost(&TmpChan, pMode);
    }
    if((pChan->SensorID & MX01_WRAPPER_SENSOR_ID_CHAN_B) != 0U) {
        TmpChan.SensorID = MX01_WRAPPER_SENSOR_ID_CHAN_B;

        RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[1]->ConfigVinPre(&TmpChan, pMode);
        RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[1]->ConfigVin(pChan->VinID+1U, &ModeInfo[1]);
        MipiVirtChanConfig.VirtChan = 0x1U;
        RetVal |= AmbaVIN_MipiVirtChanConfig(pChan->VinID+1U, &MipiVirtChanConfig);

        RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[1]->ConfigVinPost(&TmpChan, pMode);
    }
    if((pChan->SensorID & MX01_WRAPPER_SENSOR_ID_CHAN_C) != 0U) {
        TmpChan.SensorID = MX01_WRAPPER_SENSOR_ID_CHAN_C;

        RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[2]->ConfigVinPre(&TmpChan, pMode);
        RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[2]->ConfigVin(pChan->VinID+2U, &ModeInfo[2]);
        MipiVirtChanConfig.VirtChan = 0x2U;
        RetVal |= AmbaVIN_MipiVirtChanConfig(pChan->VinID+2U, &MipiVirtChanConfig);

        RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[2]->ConfigVinPost(&TmpChan, pMode);
    }
    if((pChan->SensorID & MX01_WRAPPER_SENSOR_ID_CHAN_D) != 0U) {
        TmpChan.SensorID = MX01_WRAPPER_SENSOR_ID_CHAN_D;

        RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[3]->ConfigVinPre(&TmpChan, pMode);
        RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[3]->ConfigVin(pChan->VinID+3U, &ModeInfo[3]);
        MipiVirtChanConfig.VirtChan = 0x3U;
        RetVal |= AmbaVIN_MipiVirtChanConfig(pChan->VinID+3U, &MipiVirtChanConfig);
        RetVal |= pMX01_WrapperCtrl[pChan->VinID]->pDriver[3]->ConfigVinPost(&TmpChan, pMode);
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_GetStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*GetStatus)(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_STATUS_INFO_s *pStatus) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            GetStatus = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->GetStatus;
            if (GetStatus != NULL) {
                RetVal = GetStatus(pChan, pStatus);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_GetModeInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo)
{
#if 0
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*GetModeInfo)(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            GetModeInfo = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->GetModeInfo;
            if (GetModeInfo != NULL) {
                RetVal = GetModeInfo(pChan, pMode, pModeInfo);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }
#else /* workaround for SensorID == 0x1 */
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*GetModeInfo)(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode, AMBA_SENSOR_MODE_INFO_s *pModeInfo) = NULL;
    AMBA_SENSOR_CHANNEL_s TmpChan = *pChan;

    if (TmpChan.SensorID == 0x1) {
        TmpChan.SensorID = 0x10;
    }

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((TmpChan.SensorID & (0x10U << (i << 2U))) != 0U) {
            GetModeInfo = pMX01_WrapperCtrl[TmpChan.VinID]->pDriver[i]->pSensorObj->GetModeInfo;
            if (GetModeInfo != NULL) {
                RetVal = GetModeInfo(&TmpChan, pMode, pModeInfo);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }
#endif

    return RetVal;
}

static UINT32 MX01_Wrapper_GetDeviceInfo(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*GetDeviceInfo)(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_DEVICE_INFO_s *pDeviceInfo) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            GetDeviceInfo = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->GetDeviceInfo;
            if (GetDeviceInfo != NULL) {
                RetVal = GetDeviceInfo(pChan, pDeviceInfo);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_GetHdrInfo(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*GetHdrInfo)(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl, AMBA_SENSOR_HDR_INFO_s *pHdrInfo) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            GetHdrInfo = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->GetHdrInfo;
            if (GetHdrInfo != NULL) {
                RetVal = GetHdrInfo(pChan, pShutterCtrl, pHdrInfo);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_GetCurrentGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*GetCurrentGainFactor)(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pGainFactor) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            GetCurrentGainFactor = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->GetCurrentGainFactor;
            if (GetCurrentGainFactor != NULL) {
                RetVal = GetCurrentGainFactor(pChan, pGainFactor);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_GetCurrentShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*GetCurrentShutterSpeed)(const AMBA_SENSOR_CHANNEL_s *pChan, FLOAT *pExposureTime) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            GetCurrentShutterSpeed = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->GetCurrentShutterSpeed;
            if (GetCurrentShutterSpeed != NULL) {
                RetVal = GetCurrentShutterSpeed(pChan, pExposureTime);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_ConvertGainFactor(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*ConvertGainFactor)(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_GAIN_FACTOR_s *pDesiredFactor, AMBA_SENSOR_GAIN_FACTOR_s *pActualFactor, AMBA_SENSOR_GAIN_CTRL_s *pGainCtrl) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            ConvertGainFactor = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->ConvertGainFactor;
            if (ConvertGainFactor != NULL) {
                RetVal = ConvertGainFactor(pChan, pDesiredFactor, pActualFactor, pGainCtrl);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_ConvertShutterSpeed(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*ConvertShutterSpeed)(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredExposureTime, FLOAT *pActualExposureTime, UINT32 *pShutterCtrl) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            ConvertShutterSpeed = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->ConvertShutterSpeed;
            if (ConvertShutterSpeed != NULL) {
                RetVal = ConvertShutterSpeed(pChan, pDesiredExposureTime, pActualExposureTime, pShutterCtrl);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_SetAnalogGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*SetAnalogGainCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pAnalogGainCtrl) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            SetAnalogGainCtrl = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->SetAnalogGainCtrl;
            if (SetAnalogGainCtrl != NULL) {
                RetVal = SetAnalogGainCtrl(pChan, pAnalogGainCtrl);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_SetDigitalGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*SetDigitalGainCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pDigitalGainCtrl) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            SetDigitalGainCtrl = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->SetDigitalGainCtrl;
            if (SetDigitalGainCtrl != NULL) {
                RetVal = SetDigitalGainCtrl(pChan, pDigitalGainCtrl);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_SetWbGainCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*SetWbGainCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_WB_CTRL_s *pWbGainCtrl) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            SetWbGainCtrl = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->SetWbGainCtrl;
            if (SetWbGainCtrl != NULL) {
                RetVal = SetWbGainCtrl(pChan, pWbGainCtrl);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_SetShutterCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*SetShutterCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pShutterCtrl) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            SetShutterCtrl = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->SetShutterCtrl;
            if (SetShutterCtrl != NULL) {
                RetVal = SetShutterCtrl(pChan, pShutterCtrl);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_SetSlowShrCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*SetSlowShutterCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            SetSlowShutterCtrl = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->SetSlowShutterCtrl;
            if (SetSlowShutterCtrl != NULL) {
                RetVal = SetSlowShutterCtrl(pChan, SlowShutterCtrl);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_SetMasterSyncCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*SetMasterSyncCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, UINT32 SlowShutterCtrl) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            SetMasterSyncCtrl = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->SetMasterSyncCtrl;
            if (SetMasterSyncCtrl != NULL) {
                RetVal = SetMasterSyncCtrl(pChan, SlowShutterCtrl);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_RegisterRead(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 *pData)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*RegisterRead)(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16* pData) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            RegisterRead = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->RegisterRead;
            if (RegisterRead != NULL) {
                RetVal = RegisterRead(pChan, Addr, pData);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_RegisterWrite(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*RegisterWrite)(const AMBA_SENSOR_CHANNEL_s *pChan, UINT16 Addr, UINT16 Data) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            RegisterWrite = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->RegisterWrite;
            if (RegisterWrite != NULL) {
                RetVal = RegisterWrite(pChan, Addr, Data);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_ConfigPost(const AMBA_SENSOR_CHANNEL_s *pChan)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*ConfigPost)(const AMBA_SENSOR_CHANNEL_s *pChan) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            ConfigPost = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->ConfigPost;
            if (ConfigPost != NULL) {
                RetVal = ConfigPost(pChan);
                if (RetVal == ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_GetSerdesStatus(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_SERDES_STATUS_s *pSerdesStatus)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*GetSerdesStatus)(const AMBA_SENSOR_CHANNEL_s *pChan, AMBA_SENSOR_SERDES_STATUS_s *pSerdesStatus) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            GetSerdesStatus = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->GetSerdesStatus;
            if (GetSerdesStatus != NULL) {
                RetVal = GetSerdesStatus(pChan, pSerdesStatus);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_ConvertStrobeWidth(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredWidth, FLOAT *pActualWidth, UINT32 *pStrobeCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*ConvertStrobeWidth)(const AMBA_SENSOR_CHANNEL_s *pChan, const FLOAT *pDesiredWidth, FLOAT *pActualWidth, UINT32 *pStrobeCtrl) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            ConvertStrobeWidth = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->ConvertStrobeWidth;
            if (ConvertStrobeWidth != NULL) {
                RetVal = ConvertStrobeWidth(pChan, pDesiredWidth, pActualWidth, pStrobeCtrl);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MX01_Wrapper_SetStrobeCtrl(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pStrobeCtrl)
{
    UINT32 i, RetVal = SENSOR_ERR_INVALID_API;
    UINT32 (*SetStrobeCtrl)(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 *pStrobeCtrl) = NULL;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if ((pChan->SensorID & (0x10U << (i << 2U))) != 0U) {
            SetStrobeCtrl = pMX01_WrapperCtrl[pChan->VinID]->pDriver[i]->pSensorObj->SetStrobeCtrl;
            if (SetStrobeCtrl != NULL) {
                RetVal = SetStrobeCtrl(pChan, pStrobeCtrl);
                if (RetVal != ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

AMBA_SENSOR_OBJ_s AmbaSensor_MX01_WrapperObj = {
    .SensorName             = "WRAPPER",
    .SerdesName             = "MAX9295_96712",
    .Init                   = MX01_Wrapper_Init,
    .Enable                 = MX01_Wrapper_Enable,
    .Disable                = MX01_Wrapper_Disable,
    .Config                 = MX01_Wrapper_Config,
    .GetStatus              = MX01_Wrapper_GetStatus,
    .GetModeInfo            = MX01_Wrapper_GetModeInfo,
    .GetDeviceInfo          = MX01_Wrapper_GetDeviceInfo,
    .GetHdrInfo             = MX01_Wrapper_GetHdrInfo,
    .GetCurrentGainFactor   = MX01_Wrapper_GetCurrentGainFactor,
    .GetCurrentShutterSpeed = MX01_Wrapper_GetCurrentShutterSpeed,

    .ConvertGainFactor      = MX01_Wrapper_ConvertGainFactor,
    .ConvertShutterSpeed    = MX01_Wrapper_ConvertShutterSpeed,

    .SetAnalogGainCtrl      = MX01_Wrapper_SetAnalogGainCtrl,
    .SetDigitalGainCtrl     = MX01_Wrapper_SetDigitalGainCtrl,
    .SetWbGainCtrl          = MX01_Wrapper_SetWbGainCtrl,
    .SetShutterCtrl         = MX01_Wrapper_SetShutterCtrl,
    .SetSlowShutterCtrl     = MX01_Wrapper_SetSlowShrCtrl,
    .SetMasterSyncCtrl      = MX01_Wrapper_SetMasterSyncCtrl,

    .RegisterRead           = MX01_Wrapper_RegisterRead,
    .RegisterWrite          = MX01_Wrapper_RegisterWrite,

    .ConfigPost             = MX01_Wrapper_ConfigPost,
    .GetSerdesStatus        = MX01_Wrapper_GetSerdesStatus,

    .ConvertStrobeWidth     = MX01_Wrapper_ConvertStrobeWidth,
    .SetStrobeCtrl          = MX01_Wrapper_SetStrobeCtrl,
};

void MX01_SensorHook(const AMBA_SENSOR_CHANNEL_s *pChan, MX01_SENSOR_OBJ_s *pObj[4])
{
    UINT32 i;

    for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
        if (pObj[i] != NULL) {
            pMX01_WrapperCtrl[pChan->VinID]->pDriver[i] = pObj[i];
        }
    }
};
