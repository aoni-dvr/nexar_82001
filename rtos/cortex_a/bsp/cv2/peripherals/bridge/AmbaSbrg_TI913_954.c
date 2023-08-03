/**
 *  @file AmbaSbrg_TI913_954.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree topromptly notify and
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
 *  @details TI913 & TI954 driver APIs
 *
 */

#include "AmbaI2C.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaVIN.h"

#include "AmbaSbrg_TI913_954.h"
#include "bsp.h"

static TI913_954_SERDES_CONFIG_s SerdesConfig[AMBA_NUM_VIN_CHANNEL];

static UINT32 IsInited[AMBA_NUM_VIN_CHANNEL] = {0U};
static UINT32 TI913_954I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_SENSOR_I2C_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_SENSOR_I2C_CHANNEL_PIP2,
    [AMBA_VIN_CHANNEL3] = AMBA_SENSOR_I2C_CHANNEL_PIP3,
    [AMBA_VIN_CHANNEL4] = AMBA_SENSOR_I2C_CHANNEL_PIP4,
    [AMBA_VIN_CHANNEL5] = AMBA_SENSOR_I2C_CHANNEL_PIP5
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI913_954_RegRead
 *
 *  @Description:: Read TI913_954 registers
 *
 *  @Input      ::
 *      SlaveID:   Slave id
 *      Addr:      Register address
 *      Data:      Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI913_954_RegRead(UINT32 VinID, UINT8 SlaveID, UINT8 Addr, UINT8* pRxData)
{
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[1];
    UINT32 ActualSize;
    UINT32 RetVal;

    if (pRxData == NULL) {
        RetVal = TI913_954_ERR_ARG;
    } else {
        I2cTxConfig.SlaveAddr = SlaveID;
        I2cTxConfig.DataSize  = 1U;
        I2cTxConfig.pDataBuf  = TxData;
        TxData[0] = (UINT8) (Addr & 0xffU);

        I2cRxConfig.SlaveAddr = (UINT32)SlaveID | 1U;
        I2cRxConfig.DataSize  = 1U;
        I2cRxConfig.pDataBuf  = pRxData;

        RetVal = AmbaI2C_MasterReadAfterWrite(TI913_954I2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                                              1U, &I2cTxConfig, &I2cRxConfig, &ActualSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_PrintUInt5("TI I2C read does not work!!!!! (SlaveID=0x%02x) Addr(0x%x)", SlaveID, Addr, 0U, 0U, 0U);
        }

        AmbaPrint_ModulePrintUInt5(TI913_954_MODULE_ID, "R ====> 0x%02x 0x%02x 0x%02x", SlaveID, Addr, pRxData[0], 0U, 0U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI913_954_RegWrite
 *
 *  @Description:: Write TI913_954 registers
 *
 *  @Input      ::
 *      SlaveID:   Slave id
 *      Addr:      Register address
 *      Data:      Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI913_954_RegWrite(UINT32 VinID, UINT8 SlaveID, UINT8 Addr, UINT8 Data)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig;
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cConfig.DataSize = 2U;
    I2cConfig.SlaveAddr = SlaveID;
    I2cConfig.pDataBuf = TxData;
    TxData[0] = Addr;
    TxData[1] = Data;

    RetVal = AmbaI2C_MasterWrite(TI913_954I2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                                 &I2cConfig, &TxSize, 1000U);

    if (RetVal != TI913_954_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(TI913_954_MODULE_ID, "TI I2C does not work!!!!! (SlaveID=0x%02x) Addr(0x%x) Data(0x%x)", SlaveID, Addr, Data, 0U, 0U);
    }

    AmbaPrint_ModulePrintUInt5(TI913_954_MODULE_ID, "W ====> 0x%02x 0x%02x 0x%02x", SlaveID, Addr, Data, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI913_954_Init
 *
 *  @Description:: Init TI913_954 API
 *
 *  @Input      ::
 *      pSerdesConfig:   pointer to Serdes input configuation
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 TI913_954_Init(UINT32 VinID, const TI913_954_SERDES_CONFIG_s *pSerdesConfig)
{
    UINT32 RetVal = TI913_954_ERR_NONE;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (pSerdesConfig == NULL)) {
        RetVal = TI913_954_ERR_ARG;
    } else {
        RetVal = AmbaWrap_memcpy(&SerdesConfig[VinID], pSerdesConfig, sizeof (TI913_954_SERDES_CONFIG_s));
        IsInited[VinID] = 1U;
    }
    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI913_954_Config
 *
 *  @Description:: Config TI913_954 API
 *
 *  @Input      ::
 *      VinID:     VIN ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 TI913_954_Config(UINT32 VinID)
{
    UINT32 RetVal = TI913_954_ERR_NONE;
    UINT32 Timeout = 500;
    UINT8 RxPortID;
    UINT8 ReadData;
    const TI913_954_SERDES_CONFIG_s *pSerdesConfig;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (IsInited[VinID] == 0U)) {
        RetVal = TI913_954_ERR_ARG;
    } else {
        pSerdesConfig = &SerdesConfig[VinID];
        RxPortID = (UINT8)(pSerdesConfig->EnabledRxPortID & (TI913_954_RX_PORT_0 | TI913_954_RX_PORT_1));

        AmbaPrint_ModulePrintUInt5(TI913_954_MODULE_ID, "Init TI913/954 (RxPortID: 0x%02x)...", RxPortID, 0U, 0U, 0U, 0U);

        if (RxPortID != 0U) {
            /* Enable FPD3 writes */
            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x4c, RxPortID);

            /* Enable selected 954 rx port */
            if (RxPortID == TI913_954_RX_PORT_0) {
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x0c, RxPortID);
            } else if (RxPortID == TI913_954_RX_PORT_1) {
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x0c, (UINT8)((1U << 4U) | (1U << 2U) | (RxPortID)));
            } else {
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x0c, (UINT8)((3U << 4U) | (3U << 2U) | (RxPortID)));
            }

            /* select fpd3 and coax mode */
            if (pSerdesConfig->Fpd3Mode == TI913_954_RAW12_50MHz) {
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x6d, 0x7d);
            } else if (pSerdesConfig->Fpd3Mode == TI913_954_RAW12_75MHz) {
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x6d, 0x7e);
            } else { /* TI913_954_RAW10_100MHz */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x6d, 0x7f);
            }

            /* BC pass through & BC frequency=25Mbps */
            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x58, 0x58);

            /* FPD_Link Lock status */
            while (Timeout > 0U) {
                if ((TI913_954_RegRead(VinID, pSerdesConfig->TI954Alias, 0x04, &ReadData) == OK) && ((ReadData & 0x0CU) == 0x0CU)) {
                    break;
                } else {
                    (void) AmbaKAL_TaskSleep(10);
                }
                Timeout--;
            }

            if ((ReadData & 0x0CU) != 0x0CU) {
                AmbaPrint_PrintUInt5("[NG] TI954 is unable to LOCK TI913", 0U, 0U, 0U, 0U, 0U);
                RetVal = TI913_954_ERR_ARG;
            }

            /* Set SFILTER_CFG Register */
            //RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x41, 0xA9);
            /* Restart AEQ Adaptaion */
            //RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xD2, 0x9C);

            /* Disable forwarding of RX port 0&1 */
            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x20, 0x30);

            /* Sensor framesync mode */
            if ((pSerdesConfig->FrameSyncSel.FrameSyncEnable == TI913_954_EXTERNAL_FSYNC) && (pSerdesConfig->FrameSyncSel.TI954Gpio <= TI954_GPIO_NUM)) {
                /* External framesync from TI954-GPIO-X */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x18, (0x80U | (UINT8)(pSerdesConfig->FrameSyncSel.TI954Gpio << 4U)));
            } else if (pSerdesConfig->FrameSyncSel.FrameSyncEnable == TI913_954_INTERNAL_FSYNC) {
                /* FS_CLK_PD = 12us */
                UINT32 FSyncCount, HFSyncCount, LFSyncCount;

                AmbaPrint_ModulePrintUInt5(TI913_954_MODULE_ID, "TI913_954_INTERNAL_FSYNC %d %d", pSerdesConfig->FrameSyncSel.TimeScale, pSerdesConfig->FrameSyncSel.NumUnitsInTick, 0U, 0U, 0U);
                FSyncCount = ((1000000U / pSerdesConfig->FrameSyncSel.TimeScale) * pSerdesConfig->FrameSyncSel.NumUnitsInTick) / 12U;
                HFSyncCount = (FSyncCount / 20U) - 1U; /* 5% */
                LFSyncCount = (FSyncCount - HFSyncCount) - 2U;
                /* HIGH_TIME */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x19, (UINT8)((HFSyncCount & 0xff00U) >> 8));
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x1A, (UINT8)(HFSyncCount & 0x00ffU));
                /* LOW_TIME */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x1B, (UINT8)((LFSyncCount & 0xff00U) >> 8));
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x1C, (UINT8)(LFSyncCount & 0x00ffU));
                /* FSYNC ENABLE */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x18, 0x01);
            } else {
                /* misraC check */
            }

            if ((RxPortID & TI913_954_RX_PORT_0) != 0U) {
                /* Select rx port write enable */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x4c, TI913_954_RX_PORT_0);
                /* Serdes alias ID  */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x5c, pSerdesConfig->TI913Alias[0]);
                /* Remote slave device ID0 */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x5d, pSerdesConfig->SensorSlaveID);
                /* Remote slave device ID1 */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x5e, pSerdesConfig->SensorSlaveID);
                /* Remote Slave alias ID0 */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x65, pSerdesConfig->SensorAliasID[0]);
                /* Remote Slave alias ID1 */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x66, pSerdesConfig->SensorBroadcastAliasID);

                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x7c, ((pSerdesConfig->LvPolarity & 0x1U) << 1U) | (pSerdesConfig->FvPolarity & 0x1U));

                /* Sensor framesync mode */
                if (pSerdesConfig->FrameSyncSel.FrameSyncEnable != TI913_954_NO_FSYNC) {
                    if ((pSerdesConfig->FrameSyncSel.TI954Gpio >= TI954_GPIO_NUM) || (pSerdesConfig->FrameSyncSel.TI913Gpio >= TI913_GPIO_NUM)) {
                        AmbaPrint_ModulePrintUInt5(TI913_954_MODULE_ID, "Invalid TI913/954 GPIO for frame sync", 0U, 0U, 0U, 0U, 0U);
                        RetVal |= TI913_954_ERR_ARG;
                    } else {
                        /* Select BC-TI913-GPIO-Y to connect with TI954-GPIO-X */
                        /* TI913-GPIO-Y output enable and Enable remote TI913-GPIO-Y */
                        UINT8 Data;
                        if (pSerdesConfig->FrameSyncSel.TI913Gpio == TI913_GPIO_PIN_3) {
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x6F, (0x08U | (UINT8)(pSerdesConfig->FrameSyncSel.TI954Gpio << 4U)));
                            RetVal |= TI913_954_RegRead(VinID, pSerdesConfig->TI913Alias[0], 0x0E, &Data);
                            Data &= 0x0fU;
                            Data |= 0x50U;
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI913Alias[0], 0x0E, Data);
                        } else if (pSerdesConfig->FrameSyncSel.TI913Gpio == TI913_GPIO_PIN_2) {
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x6F, (0x80U | (UINT8)pSerdesConfig->FrameSyncSel.TI954Gpio));
                            RetVal |= TI913_954_RegRead(VinID, pSerdesConfig->TI913Alias[0], 0x0E, &Data);
                            Data &= 0xf0U;
                            Data |= 0x05U;
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI913Alias[0], 0x0E, Data);
                        } else if (pSerdesConfig->FrameSyncSel.TI913Gpio == TI913_GPIO_PIN_1) {
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x6E, (0x08U | (UINT8)(pSerdesConfig->FrameSyncSel.TI954Gpio << 4U)));
                            RetVal |= TI913_954_RegRead(VinID, pSerdesConfig->TI913Alias[0], 0x0D, &Data);
                            Data &= 0x0fU;
                            Data |= 0x50U;
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI913Alias[0], 0x0D, Data);
                        } else {
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x6E, (0x80U | (UINT8)pSerdesConfig->FrameSyncSel.TI954Gpio));
                            RetVal |= TI913_954_RegRead(VinID, pSerdesConfig->TI913Alias[0], 0x0D, &Data);
                            Data &= 0xf0U;
                            Data |= 0x05U;
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI913Alias[0], 0x0D, Data);
                        }
                    }
                }
            }

            if ((RxPortID & TI913_954_RX_PORT_1) != 0U) {
                /* Select rx port write enable */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x4c, (UINT8)(0x10U | TI913_954_RX_PORT_1));
                /* Serdes alias ID  */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x5c, pSerdesConfig->TI913Alias[1]);
                /* Remote slave device ID0 */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x5d, pSerdesConfig->SensorSlaveID);
                /* Remote slave device ID1 */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x5e, pSerdesConfig->SensorSlaveID);
                /* Remote Slave alias ID0 */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x65, pSerdesConfig->SensorAliasID[1]);
                /* Remote Slave alias ID1 */
                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x66, pSerdesConfig->SensorBroadcastAliasID);

                RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x7c, ((pSerdesConfig->LvPolarity & 0x1U) << 1U) | (pSerdesConfig->FvPolarity & 0x1U));

                /* Sensor framesync mode */
                if (pSerdesConfig->FrameSyncSel.FrameSyncEnable != 0U) {
                    if ((pSerdesConfig->FrameSyncSel.TI954Gpio >= TI954_GPIO_NUM) || (pSerdesConfig->FrameSyncSel.TI913Gpio >= TI913_GPIO_NUM)) {
                        AmbaPrint_ModulePrintUInt5(TI913_954_MODULE_ID, "Invalid TI913/954 GPIO for frame sync", 0U, 0U, 0U, 0U, 0U);
                        RetVal |= TI913_954_ERR_ARG;
                    } else {
                        /* Select BC-TI913-GPIO-Y to connect with TI954-GPIO-X */
                        /* TI913-GPIO-Y output enable and Enable remote TI913-GPIO-Y */
                        UINT8 Data;
                        if (pSerdesConfig->FrameSyncSel.TI913Gpio == TI913_GPIO_PIN_3) {
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x6F, (0x08U | (UINT8)(pSerdesConfig->FrameSyncSel.TI954Gpio << 4U)));
                            RetVal |= TI913_954_RegRead(VinID, pSerdesConfig->TI913Alias[1], 0x0E, &Data);
                            Data &= 0x0fU;
                            Data |= 0x50U;
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI913Alias[1], 0x0E, Data);
                        } else if (pSerdesConfig->FrameSyncSel.TI913Gpio == TI913_GPIO_PIN_2) {
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x6F, (0x80U | (UINT8)pSerdesConfig->FrameSyncSel.TI954Gpio));
                            RetVal |= TI913_954_RegRead(VinID, pSerdesConfig->TI913Alias[1], 0x0E, &Data);
                            Data &= 0xf0U;
                            Data |= 0x05U;
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI913Alias[1], 0x0E, Data);
                        } else if (pSerdesConfig->FrameSyncSel.TI913Gpio == TI913_GPIO_PIN_1) {
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x6E, (0x08U | (UINT8)(pSerdesConfig->FrameSyncSel.TI954Gpio << 4U)));
                            RetVal |= TI913_954_RegRead(VinID, pSerdesConfig->TI913Alias[1], 0x0D, &Data);
                            Data &= 0x0fU;
                            Data |= 0x50U;
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI913Alias[1], 0x0D, Data);
                        } else {
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x6E, (0x80U | (UINT8)pSerdesConfig->FrameSyncSel.TI954Gpio));
                            RetVal |= TI913_954_RegRead(VinID, pSerdesConfig->TI913Alias[1], 0x0D, &Data);
                            Data &= 0xf0U;
                            Data |= 0x05U;
                            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI913Alias[1], 0x0D, Data);
                        }
                    }
                }
            }

            /* Select rx port write enable */
            RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x4c, RxPortID);

        } else {
            RetVal = TI913_954_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI913_954_SetSensorClk
 *
 *  @Description:: Set TI913 reference clock output for image sensor
 *
 *  @Input      ::
 *      VinID:     VIN ID
 *      pSerdesConfig:   pointer to Serdes input configuation
 *      RClkOut:   Reference clock output
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 :     OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 TI913_954_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkOut)
{
    UINT32 RetVal = TI913_954_ERR_NONE;
    UINT8 TI913Alias;
    const TI913_954_SERDES_CONFIG_s *pSerdesConfig;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (IsInited[VinID] == 0U)) {
        RetVal = TI913_954_ERR_ARG;
    } else {
        pSerdesConfig = &SerdesConfig[VinID];

        if ((ChipID & TI913_A_CHIP_ID) != 0U) {
            TI913Alias = pSerdesConfig->TI913Alias[0];
            if (RClkOut == TI913_954_RCLK_OUT_FREQ_25M) {
                RetVal = TI913_954_RegWrite(VinID, TI913Alias, 0x07, 0x28);  /* Default output clock 25MHz */

            } else if (RClkOut == TI913_954_RCLK_OUT_FREQ_27M) {
                RetVal = TI913_954_RegWrite(VinID, TI913Alias, 0x07, 0x25);  /* Adjust TI913 output clock to 27MHz */

            } else {
                AmbaPrint_ModulePrintUInt5(TI913_954_MODULE_ID, "Incorrect SensorClk option: %d", RClkOut, 0U, 0U, 0U, 0U);
                RetVal = TI913_954_ERR_ARG;
            }
        }
        if ((ChipID & TI913_B_CHIP_ID) != 0U) {
            TI913Alias = pSerdesConfig->TI913Alias[1];
            if (RClkOut == TI913_954_RCLK_OUT_FREQ_25M) {
                RetVal = TI913_954_RegWrite(VinID, TI913Alias, 0x07, 0x28);  /* Default output clock 25MHz */

            } else if (RClkOut == TI913_954_RCLK_OUT_FREQ_27M) {
                RetVal = TI913_954_RegWrite(VinID, TI913Alias, 0x07, 0x25);  /* Adjust TI913 output clock to 27MHz */

            } else {
                AmbaPrint_ModulePrintUInt5(TI913_954_MODULE_ID, "Incorrect SensorClk option: %d", RClkOut, 0U, 0U, 0U, 0U);
                RetVal = TI913_954_ERR_ARG;
            }
        }
    }

    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI913_954_SetGpioOutput
 *
 *  @Description:: Set TI913/954 GPIO Output Control
 *
 *  @Input      ::
 *      VinID:     VIN ID
 *      ChipID:    Select which TI913/954 to configure
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 :     OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 TI913_954_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT32 PinLevel)
{
    UINT32 RetVal = TI913_954_ERR_NONE;
    UINT8 GpioCtrlAddrBase = 0x10;
    UINT8 TI954Alias, TI913Alias;
    UINT8 Data;
    const TI913_954_SERDES_CONFIG_s *pSerdesConfig;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (IsInited[VinID] == 0U)) {
        RetVal = TI913_954_ERR_ARG;
    } else {
        pSerdesConfig = &SerdesConfig[VinID];
        if ((ChipID & TI954_CHIP_ID) != 0U) {
            if (PinID >= TI954_GPIO_NUM) {
                RetVal |= TI913_954_ERR_ARG;
            } else {
                TI954Alias = pSerdesConfig->TI954Alias;
                /* Enable output */
                RetVal |= TI913_954_RegRead(VinID, TI954Alias, 0x0F, &Data);
                RetVal |= TI913_954_RegWrite(VinID, TI954Alias, 0x0F, (UINT8)((~(0x1U << PinID)) & (Data & 0x7FU)));

                /* Output source */
                if (PinLevel != 0U) {
                    RetVal |= TI913_954_RegWrite(VinID, TI954Alias, (UINT8)(GpioCtrlAddrBase + PinID), (UINT8)(0x2U | 0x11U));
                } else {
                    RetVal |= TI913_954_RegWrite(VinID, TI954Alias, (UINT8)(GpioCtrlAddrBase + PinID), (UINT8)((~(0x2U)) & 0x11U));
                }
            }
        }

        if ((ChipID & TI913_A_CHIP_ID) != 0U) {
            if (PinID >= TI913_GPIO_NUM) {
                RetVal |= TI913_954_ERR_ARG;
            } else {
                TI913Alias = pSerdesConfig->TI913Alias[0];
                /* Enable output */
                RetVal |= TI913_954_RegRead(VinID, TI913Alias, 0x0E, &Data);
                RetVal |= TI913_954_RegWrite(VinID, TI913Alias, 0x0E, (UINT8)(((0x1U << PinID) << 4U) | (Data & 0xF0U)) | (UINT8)((~(0x1U << PinID)) & (Data & 0x0FU)));

                /* Output source */
                RetVal |= TI913_954_RegRead(VinID, TI913Alias, 0x0D, &Data);
                Data &= (~((0x1U << PinID) << 4U));
                if (PinLevel != 0U) {
                    RetVal |= TI913_954_RegWrite(VinID, TI913Alias, 0x0D, (UINT8)(((0x1U << PinID)) | Data));
                } else {
                    RetVal |= TI913_954_RegWrite(VinID, TI913Alias, 0x0D, (UINT8)((~(0x1U << PinID)) & Data));
                }
            }
        }

        if ((ChipID & TI913_B_CHIP_ID) != 0U) {
            if (PinID >= TI913_GPIO_NUM) {
                RetVal |= TI913_954_ERR_ARG;
            } else {
                TI913Alias = pSerdesConfig->TI913Alias[1];
                /* Enable output */
                RetVal |= TI913_954_RegRead(VinID, TI913Alias, 0x0E, &Data);
                RetVal |= TI913_954_RegWrite(VinID, TI913Alias, 0x0E, (UINT8)(((0x1U << PinID) << 4U) | (Data & 0xF0U)) | (UINT8)((~(0x1U << PinID)) & (Data & 0x0FU)));

                /* Output source */
                RetVal |= TI913_954_RegRead(VinID, TI913Alias, 0x0D, &Data);
                Data &= (~((0x1U << PinID) << 4U));
                if (PinLevel != 0U) {
                    RetVal |= TI913_954_RegWrite(VinID, TI913Alias, 0x0D, (UINT8)(((0x1U << PinID)) | Data));
                } else {
                    RetVal |= TI913_954_RegWrite(VinID, TI913Alias, 0x0D, (UINT8)((~(0x1U << PinID)) & Data));
                }
            }
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI913_954_EnableCSIOutput
 *
 *  @Description:: Enable TI954 CSI-2 output API
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 TI913_954_EnableCSIOutput(UINT32 VinID)
{
    UINT32 RetVal = TI913_954_ERR_NONE;
    UINT8 CSICTL, CSIPllCtl, FwdCtl1, FwdCtl2;
    const TI913_954_SERDES_CONFIG_s *pSerdesConfig;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (IsInited[VinID] == 0U)) {
        RetVal = TI913_954_ERR_ARG;
    } else {
        pSerdesConfig = &SerdesConfig[VinID];

        CSICTL = (UINT8)((TI913_954_NUM_CSI_LANE - pSerdesConfig->CSIOutputInfo.NumDataLanes) << 4U) | 0x43U;

        if (pSerdesConfig->CSIOutputInfo.DataRate == TI913_954_CSI_TX_SPEED_1P6G) {
            CSIPllCtl = 0x00U; /* 1.6 Gbps serial rate */
        } else if (pSerdesConfig->CSIOutputInfo.DataRate == TI913_954_CSI_TX_SPEED_800M) {
            CSIPllCtl = 0x02U; /* 800 Mbps serial rate */
        } else {
            CSIPllCtl = 0x03U; /* 400 Mbps serial rate */
        }

        if (pSerdesConfig->EnabledRxPortID == (TI913_954_RX_PORT_0 | TI913_954_RX_PORT_1)) {
            FwdCtl1 = 0x00U;
        } else if (pSerdesConfig->EnabledRxPortID == TI913_954_RX_PORT_1) {
            FwdCtl1 = 0x10U; /* Forwarding enabled for RX Port 1 */
        } else {
            FwdCtl1 = 0x20U; /* Forwarding enabled for RX Port 0 */
        }

        if (pSerdesConfig->CSIOutputInfo.ForwardingMode == TI913_954_LINE_INTERLEAVE_FWD) {
            FwdCtl2 = 0x08U; /* Synchronous forwarding with line interleaving */
        } else {
            FwdCtl2 = 0x0CU; /* Synchronous forwarding with line concatenation */
        }

        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x33, CSICTL);     // CSI2_TX Lane
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x1f, CSIPllCtl);  // CSI_TX_SPEED
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x21, FwdCtl2);    // SYNC_Forwared_Mode
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x20, FwdCtl1);    // Forward_Port_Sel
    }

    return RetVal;
}

UINT32 TI913_954_Patgen(UINT32 VinID)
{
    UINT32 RetVal = TI913_954_ERR_NONE;
    const TI913_954_SERDES_CONFIG_s *pSerdesConfig;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) || (IsInited[VinID] == 0U)) {
        RetVal |= TI913_954_ERR_ARG;
    } else {
        pSerdesConfig = &SerdesConfig[VinID];
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x33, 0x03);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x1f, 0x02);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0x20, 0x30);

        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB0, 0x02);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB1, 0x01);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x01);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x35);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x1F);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x0C);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x80);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x01);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x90);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x02);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0xD0);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x03);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x20);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x10);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x47);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x21);//0x0A);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x21);//0x0A);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0xAA);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x33);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0xF0);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x7F);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x55);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0xCC);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x0F);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x80);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x00);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x00);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x00);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x00);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x00);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x00);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x00);
        RetVal |= TI913_954_RegWrite(VinID, pSerdesConfig->TI954Alias, 0xB2, 0x00);
    }
    return RetVal;
}

