/**
 *  @file AmbaSbrg_TI953_954.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details TI953 & TI954 driver APIs
 *
 */

#include "AmbaI2C.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaVIN.h"

#include "AmbaSbrg_TI953_954.h"
#include "bsp.h"

static UINT32 TI953_954I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_TI_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_TI_I2C_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_TI_I2C_CHANNEL_PIP2,
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI953_954_RegWrite
 *
 *  @Description:: Write TI953_954 registers
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
static UINT32 TI953_954_RegWrite(UINT32 VinID, UINT8 SlaveID, UINT8 Addr, UINT8 Data)
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

    if(VinID < AMBA_NUM_VIN_CHANNEL) {
        RetVal = AmbaI2C_MasterWrite(TI953_954I2cChannel[VinID], I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);

        if (RetVal != TI953_954_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(TI953_954_MODULE_ID, "TI I2C write does not work!!!!! (SlaveID=0x%02x)", SlaveID, 0U, 0U, 0U, 0U);
        }

        AmbaPrint_ModulePrintUInt5(TI953_954_MODULE_ID, "====> 0x%02x 0x%02x 0x%02x", SlaveID, Addr, Data, 0U, 0U);
    } else {
        RetVal = TI953_954_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_RegRead
 *
 *  @Description:: Read Max9295_96712 registers
 *
 *  @Input      ::
 *      SlaveID:   Slave id
 *      Addr:      Register address
 *      pRxData:   Pointer to register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI953_954_RegRead(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[1];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = SlaveID;
    I2cTxConfig.DataSize  = 1U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8) (Addr & 0xffU);

    I2cRxConfig.SlaveAddr = (UINT32)SlaveID | (UINT32)1U;
    I2cRxConfig.DataSize  = 1U;
    I2cRxConfig.pDataBuf  = pRxData;

    RetVal = AmbaI2C_MasterReadAfterWrite(TI953_954I2cChannel[VinID], I2C_SPEED_STANDARD, 1U,
                                          &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U);


    if (RetVal != TI953_954_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(TI953_954_MODULE_ID, "TI I2C read does not work!!!!! (SlaveID=0x%02x)", SlaveID, 0U, 0U, 0U, 0U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_WaitLockStatus
 *
 *  @Description:: Wait for Max9295_96712 link to be established
 *
 *  @Input      ::
 *      SlaveID:   Slave id
 *      TimeOut:   Timeout (in ms)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 TI953_954_WaitLockStatus(UINT32 VinID, UINT8 SlaveID, UINT32 TimeOut, UINT16 EnabledLinkID)
{
    UINT32 RetVal = TI953_954_ERR_NONE;
    UINT32 Count = 0U;
    UINT8 RxData = 0U;
    UINT8 Lock = 0U; //4'b1111, 1: Lock OK, 0: Lock NG


    AmbaPrint_ModulePrintUInt5(TI953_954_MODULE_ID, "[TI953_954] Please ignore \"I2C does not work\" msg before locked to wait for i2c communication", 0U, 0U, 0U, 0U, 0U);
    while ((Lock != EnabledLinkID) && (Count != TimeOut)) {

        if(((EnabledLinkID & 0x1U) == 0x1U) && ((Lock & 0x1U) != 0x1U)) {
            RetVal = TI953_954_RegRead(VinID, SlaveID, 0x04U, &RxData);

            if ((RetVal == OK) && ((RxData & 0x0CU) == 0x0CU)) {
                Lock |= 0x1U;
            }
        }

        (void)AmbaKAL_TaskSleep(1U);
        Count++;
    }

    if((EnabledLinkID & 0x1U) != 0U) {
        if((Lock & 0x1U) != 0U) {
            AmbaPrint_ModulePrintUInt5(TI953_954_MODULE_ID, "[TI953_954] TI954 LinkA locked! Reg0x04 = 0x%02x", RxData, 0U, 0U, 0U, 0U);
        } else {
            AmbaPrint_ModulePrintUInt5(TI953_954_MODULE_ID, "[TI953_954] TI954 LinkA NOT locked! Reg0x04 = 0x%02x", RxData, 0U, 0U, 0U, 0U);
            RetVal = TI953_954_ERR_SERDES_LINK;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI953_954_Init
 *
 *  @Description:: Init TI953_954 API
 *
 *  @Input      ::
 *      pSerdesConfig:   pointer to Serdes input configuation
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 TI953_954_Init(UINT32 VinID, UINT16 RxPortID)
{
    UINT32 RetVal = TI953_954_ERR_NONE;
    UINT8  TI954SlaveID;

    if ((VinID == 0U) || (VinID == 2U) || (VinID == 3U)) {
        TI954SlaveID = IDC_ADDR_TI954_A;
    } else { /* VinID == 1U or 4U or 5U */
        TI954SlaveID = IDC_ADDR_TI954_B;
    }

    if ((RxPortID & TI953_954_RX_PORT_0) != 0U) {
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x4c, 0x01); /* FPD3 write enable for RX port0 */
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x0f, 0x7d); /* Disable TI954 GPIO1 input */

        /* Set TI954 GPIO 1 output low -> high to power on TI953 */
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x11, 0x11);
        (void)AmbaKAL_TaskSleep(10);
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x11, 0x13);
        (void)AmbaKAL_TaskSleep(20);

    }
    if ((RxPortID & TI953_954_RX_PORT_1) != 0U) {
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x4c, 0x12); /* FPD3 write enable for RX port1 */
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x0f, 0x7e); /* Disable TI954 GPIO0 input */

        /* Set TI954 GPIO 0 output low -> high to power on TI953 */
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x10, 0x11);
        (void)AmbaKAL_TaskSleep(10);
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x10, 0x13);
        (void)AmbaKAL_TaskSleep(20);

    }

    return RetVal;
}

UINT32 TI953_954_Config(UINT32 VinID, const TI953_954_SERDES_CONFIG_s *pSerdesConfig)
{
    UINT32 RetVal = TI953_954_ERR_NONE;
    UINT8  SensorSlaveAddr;
    UINT8  SensorAliasID;
    UINT8  TI954SlaveID;
    UINT8  TI953Alias[TI954_NUM_RX_PORT];
    UINT16 RxPortID;
    UINT8 FwdEnable = 0x30U;  /* Disable both RxPort as default */

    if ((pSerdesConfig == NULL) || (VinID >= AMBA_NUM_VIN_CHANNEL)) {
        RetVal = TI953_954_ERR_ARG;
    } else {
        if ((VinID == 0U) || (VinID == 2U)) {
            TI954SlaveID = IDC_ADDR_TI954_A;
            TI953Alias[0] = IDC_ADDR_TI953_A_LINK0;
            TI953Alias[1] = IDC_ADDR_TI953_A_LINK1;
        } else {  /* VinID == 1U */
            TI954SlaveID = IDC_ADDR_TI954_B;
            TI953Alias[0] = IDC_ADDR_TI953_B_LINK0;
            TI953Alias[1] = IDC_ADDR_TI953_B_LINK1;
        }

        RxPortID = pSerdesConfig->EnabledRxPortID;
        SensorSlaveAddr = pSerdesConfig->SensorSlaveID;

        if ((RxPortID & TI953_954_RX_PORT_0) != 0U) {
            SensorAliasID = pSerdesConfig->SensorAliasID[0];

            (void) TI953_954_WaitLockStatus(VinID, TI954SlaveID, 5000U, pSerdesConfig->EnabledRxPortID);

            RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x58, 0x5e);                     /* BC pass through & BC frequency = 50Mbps */

            RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x5c, TI953Alias[0]);            /* Serdes alias ID */

            if ((VinID == 0U) || (VinID == 2U) || (VinID == 3U)) {
                RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x5d, SensorSlaveAddr);      /* Remote slave device ID0 */
                RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x65, SensorAliasID);        /* Remote Slave alias ID0 */
            } else {  /* VinID == 1U or 4U or 5U */
                RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x5f, SensorSlaveAddr);      /* Remote slave device ID2 */
                RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x67, SensorAliasID);        /* Remote Slave alias ID2 */
            }

            RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x7c, 0x00);       /* FV_POLARITY */
            RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x18, 0xa0);       /* External framesync from TI954 GPIO2 */
            RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x6F, 0x28);       /* Select BC-GPIO3 to connect with TI954 GPIO2 */
            (void)AmbaKAL_TaskSleep(100);

            RetVal |= TI953_954_RegWrite(VinID, TI953Alias[0], 0x02, 0x73);      /* Receive sensor output continuous clock */
            RetVal |= TI953_954_RegWrite(VinID, TI953Alias[0], 0x0e, 0xc0);      /* Enable GPIO2 and GPIO3 output of TI953 (GPIO2:SEN_RST, GPIO3:FSync) */
            RetVal |= TI953_954_RegWrite(VinID, TI953Alias[0], 0x0d, 0x84);      /* Enable remote GPIO3 and output high on GPIO2 of TI953 */
        }
        if ((RxPortID & TI953_954_RX_PORT_1) != 0U) {
            SensorAliasID = pSerdesConfig->SensorAliasID[1];

            RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x58, 0x5e);                     /* BC pass through & BC frequency = 50Mbps */

            RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x5c, TI953Alias[1]);            /* Serdes alias ID */

            if ((VinID == 0U) || (VinID == 2U) || (VinID == 3U)) {
                RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x5e, SensorSlaveAddr);      /* Remote slave device ID1 */
                RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x66, SensorAliasID);        /* Remote Slave alias ID1 */
            } else {  /* VinID == 1U or 4U or 5U */
                RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x60, SensorSlaveAddr);      /* Remote slave device ID3 */
                RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x68, SensorAliasID);        /* Remote Slave alias ID3 */
            }

            RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x7c, 0x00);       /* FV_POLARITY */
            RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x18, 0xa0);       /* External framesync from TI954 GPIO2 */
            RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x6F, 0x28);       /* Select BC-GPIO3 to connect with TI954 GPIO2 */
            (void)AmbaKAL_TaskSleep(100);

            RetVal |= TI953_954_RegWrite(VinID, TI953Alias[1], 0x02, 0x73);      /* Receive sensor output continuous clock */
            RetVal |= TI953_954_RegWrite(VinID, TI953Alias[1], 0x0e, 0xc0);      /* Enable GPIO2 and GPIO3 output of TI953 (GPIO2:SEN_RST, GPIO3:FSync) */
            RetVal |= TI953_954_RegWrite(VinID, TI953Alias[1], 0x0d, 0x84);      /* Enable remote GPIO3 and output high on GPIO2 of TI953 */
        }

        FwdEnable ^= (UINT8)(RxPortID << 4U);
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x20, FwdEnable);      /* Select RxPort for output source */
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI953_954_SetSensorClk
 *
 *  @Description:: Set TI953 reference clock output for image sensor
 *
 *  @Input      ::
 *      VinID:     VIN ID
 *      ChipID:    Chip ID
 *      RClkOut:   Reference clock output
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 :     OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 TI953_954_SetSensorClk(UINT32 VinID, UINT16 RxPortID, UINT32 RClkOut)
{
    UINT32 RetVal = TI953_954_ERR_NONE;
    UINT8  TI954SlaveID;
    UINT8  TI953Alias[TI954_NUM_RX_PORT];

    if ((VinID == 0U) || (VinID == 2U) || (VinID == 3U)) {
        TI954SlaveID = IDC_ADDR_TI954_A;
        TI953Alias[0] = IDC_ADDR_TI953_A_LINK0;
        TI953Alias[1] = IDC_ADDR_TI953_A_LINK1;
    } else {  /* VinID == 1U or 4U or 5U */
        TI954SlaveID = IDC_ADDR_TI954_B;
        TI953Alias[0] = IDC_ADDR_TI953_B_LINK0;
        TI953Alias[1] = IDC_ADDR_TI953_B_LINK1;
    }

    if ((RxPortID & TI953_954_RX_PORT_0) != 0U) {
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x5c, TI953Alias[0]);
        if (RClkOut == TI953_954_RCLK_OUT_FREQ_25M) {
            RetVal |= TI953_954_RegWrite(VinID, TI953Alias[0], 0x07, 0x28);  /* Default output clock 25MHz */
        } else if (RClkOut == TI953_954_RCLK_OUT_FREQ_27M) {
            RetVal |= TI953_954_RegWrite(VinID, TI953Alias[0], 0x07, 0x25);  /* Adjust TI953 output clock to 27MHz */
        } else {
            AmbaPrint_ModulePrintUInt5(TI953_954_MODULE_ID, "[TI953_954_SetSensorClk]Incorrect SensorClk option: %d", RClkOut, 0U, 0U, 0U, 0U);
            RetVal |= TI953_954_ERR_ARG;
        }
    }
    if ((RxPortID & TI953_954_RX_PORT_1) != 0U) {
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x5c, TI953Alias[1]);
        if (RClkOut == TI953_954_RCLK_OUT_FREQ_25M) {
            RetVal |= TI953_954_RegWrite(VinID, TI953Alias[1], 0x07, 0x28);  /* Default output clock 25MHz */
        } else if (RClkOut == TI953_954_RCLK_OUT_FREQ_27M) {
            RetVal |= TI953_954_RegWrite(VinID, TI953Alias[1], 0x07, 0x25);  /* Adjust TI953 output clock to 27MHz */
        } else {
            AmbaPrint_ModulePrintUInt5(TI953_954_MODULE_ID, "[TI953_954_SetSensorClk]Incorrect SensorClk option: %d", RClkOut, 0U, 0U, 0U, 0U);
            RetVal |= TI953_954_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: TI953_954_EnableCSIOutput
 *
 *  @Description:: Enable TI954 CSI-2 output API
 *
 *  @Input      ::
 *      pSerdesOutputConfig:    pointer to Serdes output configuation
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 TI953_954_EnableCSIOutput(UINT32 VinID, const TI953_954_SERDES_OUTPUT_CONFIG_s *pSerdesOutputConfig)
{
    UINT32 RetVal = TI953_954_ERR_NONE;
    UINT8 CSICTL, CSIPllCtl, FwdMode;
    UINT8  TI954SlaveID;

    if ((VinID == 0U) || (VinID == 2U) || (VinID == 3U)) {
        TI954SlaveID = IDC_ADDR_TI954_A;
    } else { /* VinID == 1U or 4U or 5U */
        TI954SlaveID = IDC_ADDR_TI954_B;
    }

    if (pSerdesOutputConfig == NULL) {
        RetVal = TI953_954_ERR_ARG;
    } else {
        CSICTL = (UINT8)((TI953_954_NUM_CSI_LANE - pSerdesOutputConfig->CSILaneNum) << 4U) | 0x43U;

        if (pSerdesOutputConfig->CSITxSpeed == TI953_954_CSI_TX_SPEED_1P6G) {
            CSIPllCtl = 0x00U;  /* 1.6 Gbps serial rate */
        } else if (pSerdesOutputConfig->CSITxSpeed == TI953_954_CSI_TX_SPEED_800M) {
            CSIPllCtl = 0x02U;  /* 800 Mbps serial rate */
        } else {
            CSIPllCtl = 0x03U;  /* 400 Mbps serial rate */
        }

        if (pSerdesOutputConfig->ForwardingMode == TI953_954_LINE_INTERLEAVE_FWD) {
            FwdMode = 0x08U; /* Synchronous forwarding with line interleaving */
        } else {
            FwdMode = 0x0CU; /* Synchronous forwarding with line concatenation */
        }

        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x33, CSICTL);
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x1f, CSIPllCtl);
        RetVal |= TI953_954_RegWrite(VinID, TI954SlaveID, 0x21, FwdMode);
        (void) AmbaKAL_TaskSleep(10);
    }

    return RetVal;
}
