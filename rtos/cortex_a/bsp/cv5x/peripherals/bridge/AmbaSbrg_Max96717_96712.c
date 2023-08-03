/**
 *  @file AmbaSbrg_Max96717_96712.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details MAX96717 & MAX96712 driver APIs
 *
 */

#include <AmbaWrap.h>
#include "AmbaKAL.h"
#include "AmbaI2C.h"
#include "AmbaPrint.h"
#include "AmbaVIN.h"
#include "AmbaSbrg_Max96717_96712.h"

#include "bsp.h"

#define MAX96712_MAX_NUM        AMBA_NUM_VIN_CHANNEL
#define SERIALIZER_BC       // Serializer broadcast I2c slave address

typedef struct {
    UINT32 InitDone;
    UINT8  DesAddr;          /* Max96712 I2C Slave Addr */
    UINT8  SerAddr0;         /* Distinguishable MAX96717 LinkA I2C Slave Addr */
    UINT8  SerAddr1;         /* Distinguishable MAX96717 LinkB I2C Slave Addr */
    UINT8  SerAddr2;         /* Distinguishable MAX96717 LinkC I2C Slave Addr */
    UINT8  SerAddr3;         /* Distinguishable MAX96717 LinkD I2C Slave Addr */
    UINT8  EnabledLinkID;
} MAX96717_96712_SERDES_INFO_s;

typedef struct {
    UINT32  RClkSel:     2; /* [1:0] MAX96717 RCLKOUT clock selection */
    UINT32  RClkAlt:     1; /* [2] MAX96717 Select MFP pin to route RCLK to */
    UINT32  Reserved:    29;/* for misrac compliant */
} MAX96717_REG3_RCLK_s;

static UINT32 MAX96717_96712I2cChannel[MAX96712_MAX_NUM] = {
    [AMBA_VIN_CHANNEL0]  = AMBA_MAXIM_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1]  = AMBA_MAXIM_I2C_CHANNEL_VIN1,
    [AMBA_VIN_CHANNEL2]  = AMBA_MAXIM_I2C_CHANNEL_VIN2,
    [AMBA_VIN_CHANNEL3]  = AMBA_MAXIM_I2C_CHANNEL_VIN3,
    [AMBA_VIN_CHANNEL4]  = AMBA_MAXIM_I2C_CHANNEL_VIN4,
    [AMBA_VIN_CHANNEL5]  = AMBA_MAXIM_I2C_CHANNEL_VIN5,
    [AMBA_VIN_CHANNEL6]  = AMBA_MAXIM_I2C_CHANNEL_VIN6,
    [AMBA_VIN_CHANNEL7]  = AMBA_MAXIM_I2C_CHANNEL_VIN7,
    [AMBA_VIN_CHANNEL8]  = AMBA_MAXIM_I2C_CHANNEL_VIN8,
    [AMBA_VIN_CHANNEL9]  = AMBA_MAXIM_I2C_CHANNEL_VIN9,
    [AMBA_VIN_CHANNEL10] = AMBA_MAXIM_I2C_CHANNEL_VIN10,
    [AMBA_VIN_CHANNEL11] = AMBA_MAXIM_I2C_CHANNEL_VIN11,
    [AMBA_VIN_CHANNEL12] = AMBA_MAXIM_I2C_CHANNEL_VIN12,
    [AMBA_VIN_CHANNEL13] = AMBA_MAXIM_I2C_CHANNEL_VIN13,
};

static MAX96717_96712_SERDES_INFO_s MaxSerDesInfo[MAX96712_MAX_NUM] = {0};
static UINT32 Max96717_96712_SetDefaultConfig(UINT32 VinID, const MAX96717_96712_SERDES_CONFIG_s *pSerdesConfig);

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96717_96712_RegWrite
 *
 *  @Description:: Write Max96717_96712 registers
 *
 *  @Input      ::
 *      SlaveID:   Slave id
 *      Addr:      Register address
 *      Data:      Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 Max96717_96712_RegWrite(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 Data)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[3];
    UINT32 TxSize;

    I2cConfig.SlaveAddr = SlaveID;
    I2cConfig.DataSize  = 3U;
    I2cConfig.pDataBuf  = TxDataBuf;

    TxDataBuf[0] = (UINT8) (Addr >> 8U);
    TxDataBuf[1] = (UINT8) (Addr & 0xffU);
    TxDataBuf[2] = Data;

    if (AmbaI2C_MasterWrite(MAX96717_96712I2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                      &I2cConfig, &TxSize, 1000U) != OK) {
        AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "SlaveID 0x%02x Addr 0x%04x I2C does not work!!!!!", SlaveID, Addr, 0U, 0U, 0U);
        RetVal = MAX96717_96712_ERR_COMMUNICATE;
    } else {
        AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712 Ch%d W] %02X %04X %02X", MAX96717_96712I2cChannel[VinID], SlaveID, Addr, Data, 0U);
        RetVal = MAX96717_96712_ERR_NONE;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96717_96712_RegRead
 *
 *  @Description:: Read Max96717_96712 registers
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
static UINT32 Max96717_96712_RegRead(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = SlaveID;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8) (Addr >> 8U);
    TxData[1] = (UINT8) (Addr & 0xffU);

    I2cRxConfig.SlaveAddr = (UINT32)SlaveID | 1U;
    I2cRxConfig.DataSize  = 1U;
    I2cRxConfig.pDataBuf  = pRxData;

    if (AmbaI2C_MasterReadAfterWrite(MAX96717_96712I2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                               1U, &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U) != OK) {
        AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "SlaveID 0x%02x Addr 0x%04x I2C does not work!!!!!", SlaveID, Addr, 0U, 0U, 0U);
        RetVal = MAX96717_96712_ERR_COMMUNICATE;
    } else {
        // AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712 Ch%d R] %02X %02X %02X", MAX96717_96712I2cChannel[VinID], SlaveID, Addr, pRxData[0U], 0U);
        RetVal = MAX96717_96712_ERR_NONE;
    }

    return RetVal;
}

static UINT32 Max96717_96712_RegReadModifyWrite(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 Data, UINT8 Mask)
{
    UINT32 RetVal;
    UINT8 RData;

    RetVal = Max96717_96712_RegRead(VinID, SlaveID, Addr, &RData);

    if (RetVal == MAX96717_96712_ERR_NONE) {
        RetVal = Max96717_96712_RegWrite(VinID, SlaveID, Addr, Data | (RData & ~Mask));
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96717_96712_WaitLockStatus
 *
 *  @Description:: Wait for Max96717_96712 link to be established
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
static UINT32 Max96717_96712_WaitLockStatus(UINT32 VinID, UINT8 SlaveID, UINT16 EnabledLinkID, UINT32 TimeOut)
{
    UINT32 RetVal = MAX96717_96712_ERR_NONE;
    UINT32 Count = 0U;
    UINT8 RxData;

    AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "Max96717_96712_WaitLockStatus: SlaveID 0x%02x, EnabledLinkID=0x%x", SlaveID, EnabledLinkID, 0U, 0U, 0U);

    if ((EnabledLinkID & 0x1U) > 0U) {
        RxData = 0x0U;
        while (((RxData & 0x08U) == 0x0U) && (Count < TimeOut)) {

            RetVal |= Max96717_96712_RegRead(VinID, SlaveID, 0x001AU, &RxData);
            if (RetVal != MAX96717_96712_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] GMSL2 LinkA WaitLockStatus fail", 0U, 0U, 0U, 0U, 0U);
                break;
            }

            (void) AmbaKAL_TaskSleep(1);
            Count++;
        }

        if (Count == TimeOut) {
            AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] 96712 Reg0x001A=0x%02x (%d) - GMSL2 LinkA NOT locked!", RxData, Count, 0U, 0U, 0U);
            RetVal |= MAX96717_96712_ERR_SERDES_LINK;
        } else {
            AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] 96712 Reg0x001A=0x%02x (%d) - GMSL2 LinkA Locked!", RxData, Count, 0U, 0U, 0U);
        }
    }

    if ((EnabledLinkID & 0x2U) > 0U) {
        RxData = 0x0U;
        while (((RxData & 0x08U) == 0x0U) && (Count < TimeOut)) {

            RetVal |= Max96717_96712_RegRead(VinID, SlaveID, 0x000AU, &RxData);
            if (RetVal != MAX96717_96712_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] GMSL2 LinkB WaitLockStatus fail", 0U, 0U, 0U, 0U, 0U);
                break;
            }

            (void) AmbaKAL_TaskSleep(1);
            Count++;
        }

        if (Count == TimeOut) {
            AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] 96712 Reg0x000A=0x%02x (%d) - GMSL2 LinkB NOT locked!", RxData, Count, 0U, 0U, 0U);
            RetVal |= MAX96717_96712_ERR_SERDES_LINK;
        } else {
            AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] 96712 Reg0x000A=0x%02x (%d) - GMSL2 LinkB Locked!", RxData, Count, 0U, 0U, 0U);
        }
    }

    if ((EnabledLinkID & 0x4U) > 0U) {
        RxData = 0x0U;
        while (((RxData & 0x08U) == 0x0U) && (Count < TimeOut)) {

            RetVal |= Max96717_96712_RegRead(VinID, SlaveID, 0x000BU, &RxData);
            if (RetVal != MAX96717_96712_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] GMSL2 LinkC WaitLockStatus fail", 0U, 0U, 0U, 0U, 0U);
                break;
            }

            (void) AmbaKAL_TaskSleep(1);
            Count++;
        }

        if (Count == TimeOut) {
            AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] 96712 Reg0x000B=0x%02x (%d) - GMSL2 LinkC NOT locked!", RxData, Count, 0U, 0U, 0U);
            RetVal |= MAX96717_96712_ERR_SERDES_LINK;
        } else {
            AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] 96712 Reg0x000B=0x%02x (%d) - GMSL2 LinkC Locked!", RxData, Count, 0U, 0U, 0U);
        }
    }

    if ((EnabledLinkID & 0x8U) > 0U) {
        RxData = 0x0U;
        while (((RxData & 0x08U) == 0x0U) && (Count < TimeOut)) {

            RetVal |= Max96717_96712_RegRead(VinID, SlaveID, 0x000CU, &RxData);
            if (RetVal != MAX96717_96712_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] GMSL2 LinkD WaitLockStatus fail", 0U, 0U, 0U, 0U, 0U);
                break;
            }

            (void) AmbaKAL_TaskSleep(1);
            Count++;
        }

        if (Count == TimeOut) {
            AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] 96712 Reg0x000C=0x%02x (%d) - GMSL2 LinkD NOT locked!", RxData, Count, 0U, 0U, 0U);
            RetVal |= MAX96717_96712_ERR_SERDES_LINK;
        } else {
            AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] 96712 Reg0x000C=0x%02x (%d) - GMSL2 LinkD Locked!", RxData, Count, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

UINT32 Max96717_96712_DeInit(UINT32 VinID)
{
    MAX96717_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    pCurSerDesInfo->InitDone = 0;

    return MAX96717_96712_ERR_NONE;
}

UINT32 Max96717_96712_Init(UINT32 VinID, UINT16 EnabledLinkID, UINT8 ForwardLinkRate)
{
    MAX96717_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX96717_96712_ERR_NONE;
    UINT32 i;
    UINT8 Data = 0U;
    // UINT8 LinkLocked[MAX96712_NUM_RX_PORT] = {0};
    // UINT8 CurDesInitDone = 0U;

    static UINT8 IdcAddrMax96712[MAX96712_MAX_NUM] = {
        [AMBA_VIN_CHANNEL0]  = MX07_IDC_ADDR_MAX96712_0,
        [AMBA_VIN_CHANNEL4]  = MX07_IDC_ADDR_MAX96712_4,
        [AMBA_VIN_CHANNEL8]  = MX07_IDC_ADDR_MAX96712_8,
        [AMBA_VIN_CHANNEL11] = MX07_IDC_ADDR_MAX96712_11,
    };

    static UINT8 IdcAddrMAX96717[MAX96712_MAX_NUM][MAX96712_NUM_RX_PORT] = {
        [AMBA_VIN_CHANNEL0]  = {MX07_IDC_ADDR_MAX96717_0_A,  MX07_IDC_ADDR_MAX96717_0_B,  MX07_IDC_ADDR_MAX96717_0_C,  MX07_IDC_ADDR_MAX96717_0_D},
        [AMBA_VIN_CHANNEL4]  = {MX07_IDC_ADDR_MAX96717_4_A,  MX07_IDC_ADDR_MAX96717_4_B,  MX07_IDC_ADDR_MAX96717_4_C,  MX07_IDC_ADDR_MAX96717_4_D},
        [AMBA_VIN_CHANNEL8]  = {MX07_IDC_ADDR_MAX96717_8_A,  MX07_IDC_ADDR_MAX96717_8_B,  MX07_IDC_ADDR_MAX96717_8_C,  MX07_IDC_ADDR_MAX96717_8_D},
        [AMBA_VIN_CHANNEL11] = {MX07_IDC_ADDR_MAX96717_11_A, MX07_IDC_ADDR_MAX96717_11_B, MX07_IDC_ADDR_MAX96717_11_C, MX07_IDC_ADDR_MAX96717_11_D},
    };

    /* LinkA or LinkB or LinkA & LinkB */
    // EnabledLinkID &= 0x3U;
    // if (EnabledLinkID != 1U) { //TBD
    //     AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] NG, only support 1ch MAX96712/96717", 0U, 0U, 0U, 0U, 0U);
    //     RetVal = MAX96717_96712_ERR_ARG;
    // }

    pCurSerDesInfo->DesAddr = IdcAddrMax96712[VinID];
    pCurSerDesInfo->SerAddr0 = IdcAddrMAX96717[VinID][0];
    pCurSerDesInfo->SerAddr1 = IdcAddrMAX96717[VinID][1];
    pCurSerDesInfo->SerAddr2 = IdcAddrMAX96717[VinID][2];
    pCurSerDesInfo->SerAddr3 = IdcAddrMAX96717[VinID][3];
    pCurSerDesInfo->EnabledLinkID |= (UINT8) EnabledLinkID;

    AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] Init, Vin(%d), Link(%d) to be built, Total Link(%d) on DES, only support 1ch LinkA for sensor master mode", VinID, EnabledLinkID, pCurSerDesInfo->EnabledLinkID, 0U, 0U);

    AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] De-serializer Initialization", pCurSerDesInfo->DesAddr, 0U, 0U, 0U, 0U);
    /* Set forward link Rx rate for GMSL2 */
    if ((ForwardLinkRate == MAX96717_96712_FORWARD_LINK_3GBPS) || (ForwardLinkRate == MAX96717_96712_FORWARD_LINK_6GBPS)) {
        Data = ForwardLinkRate * 0x11U;
        RetVal |= Max96717_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, Data);
        RetVal |= Max96717_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0011U, Data);
    }

    /* One-shot link reset for EnabledLinkID */
    RetVal |= Max96717_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0018U, 0x0F);
    (void)AmbaKAL_TaskSleep(60); // LOCK takes at least 20ms after PWDN
    if (Max96717_96712_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, EnabledLinkID, 200U) != MAX96717_96712_ERR_NONE) {
        RetVal = MAX96717_96712_ERR_SERDES_LINK;
    } else {
        AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] Set MAX96712 GMSL2 Link rate %dGbps and check locked ok", (UINT32)(ForwardLinkRate * 3U), 0U, 0U, 0U, 0U);
    }

    AmbaPrint_ModulePrintStr5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] Please ignore i2c failed for 1st bootup or not connect to those MAX96717", NULL, NULL, NULL, NULL, NULL);

    /* if modified MAX96717 i2c slave addr existed =>  reset MAX96717 for all (not first boot-up case) */
    if (Max96717_96712_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0000U, &Data) == MAX96717_96712_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] reset MAX96717 LinkA for not power off/on case", NULL, NULL, NULL, NULL, NULL);
        // set MFP2 as GPIO low to make CFG1 works fine when reset MAX96717
        // disable MAX96717 clk_si
        (void) Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x03U, 0x3U);
        // set MFP2 as GPIO low
        (void) Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x2C4U, 0x80U);
        // MAX96717 reset all
        (void) Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x10U, 0x81U); //TBD: 0x81 for LinkA only by FAE
    } else {
        // not reset LinkA Max96717
    }

    if (Max96717_96712_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0000U, &Data) == MAX96717_96712_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] reset MAX96717 LinkB for not power off/on case", NULL, NULL, NULL, NULL, NULL);
        // set MFP2 as GPIO low to make CFG1 works fine when reset MAX96717
        // disable MAX96717 clk_si
        (void) Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x03U, 0x3U);
        // set MFP2 as GPIO low
        (void) Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x2C4U, 0x80U);
        // MAX96717 reset all
        (void) Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x10U, 0x81U); //TBD: 0x81 for LinkB only by FAE
    } else {
        // not reset LinkB Max96717
    }

    if (Max96717_96712_RegRead(VinID, pCurSerDesInfo->SerAddr2, 0x0000U, &Data) == MAX96717_96712_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] reset MAX96717 LinkC for not power off/on case", NULL, NULL, NULL, NULL, NULL);
        // set MFP2 as GPIO low to make CFG1 works fine when reset MAX96717
        // disable MAX96717 clk_si
        (void) Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr2, 0x03U, 0x3U);
        // set MFP2 as GPIO low
        (void) Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr2, 0x2C4U, 0x80U);
        // MAX96717 reset all
        (void) Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr2, 0x10U, 0x81U); //TBD: 0x81 for LinkC only by FAE
    } else {
        // not reset LinkC Max96717
    }

    if (Max96717_96712_RegRead(VinID, pCurSerDesInfo->SerAddr3, 0x0000U, &Data) == MAX96717_96712_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] reset MAX96717 LinkD for not power off/on case", NULL, NULL, NULL, NULL, NULL);
        // set MFP2 as GPIO low to make CFG1 works fine when reset MAX96717
        // disable MAX96717 clk_si
        (void) Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr3, 0x03U, 0x3U);
        // set MFP2 as GPIO low
        (void) Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr3, 0x2C4U, 0x80U);
        // MAX96717 reset all
        (void) Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr3, 0x10U, 0x81U); //TBD: 0x81 for LinkD only by FAE
    } else {
        // not reset LinkD Max96717
    }

    // Check locked
    (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
    if (Max96717_96712_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, EnabledLinkID, 200U) != MAX96717_96712_ERR_NONE) {
        RetVal = MAX96717_96712_ERR_SERDES_LINK;
    } else {
        AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] Init Lock OK", 0U, 0U, 0U, 0U, 0U);
    }

    /* I2C slave address translation */
    AmbaPrint_ModulePrintStr5(MAX96717_96712_MODULE_ID, "; I2C slave address translation", NULL, NULL, NULL, NULL, NULL);
    for (i = 0U; i < MAX96712_NUM_RX_PORT; i ++) {
        if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
            Data = (0xF0U) | ((UINT8)1U << i);
            //Enable Link X Only and change Link X MAX96717A slave address to
            RetVal |= Max96717_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0006U, Data);
            (void)AmbaKAL_TaskSleep(100U);

            if (Max96717_96712_RegRead(VinID, MX07_IDC_ADDR_MAX96717_DEFAULT, 0x0000U, &Data) == MAX96717_96712_ERR_NONE) {
                RetVal |= Max96717_96712_RegWrite(VinID, MX07_IDC_ADDR_MAX96717_DEFAULT, 0x0000U, (UINT8)IdcAddrMAX96717[VinID][i]);
                AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "VinID(%d) Remap Ser SlaveID: 0x%x -> 0x%x", VinID, MX07_IDC_ADDR_MAX96717_DEFAULT, IdcAddrMAX96717[VinID][i], 0U, 0U);
            } else if (Max96717_96712_RegRead(VinID, MX07_IDC_ADDR_MAX96717_DEFAULT2, 0x0000U, &Data) == MAX96717_96712_ERR_NONE) {
                RetVal |= Max96717_96712_RegWrite(VinID, MX07_IDC_ADDR_MAX96717_DEFAULT2, 0x0000U, (UINT8)IdcAddrMAX96717[VinID][i]);
                AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "VinID(%d) Remap Ser SlaveID: 0x%x -> 0x%x", VinID, MX07_IDC_ADDR_MAX96717_DEFAULT2, IdcAddrMAX96717[VinID][i], 0U, 0U);
            } else {
                RetVal = MAX96717_96712_ERR_SERDES_LINK;
            }
            (void)AmbaKAL_TaskSleep(100U);
        }
    }
    if ((EnabledLinkID & 0xEU) != 0U) {
        AmbaPrint_ModulePrintStr5(MAX96717_96712_MODULE_ID, "; Re-enable multi-ch", NULL, NULL, NULL, NULL, NULL);
        Data = (UINT8)(0xF0U + (EnabledLinkID & 0xFU));
        RetVal |= Max96717_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0006U, Data);
        (void)AmbaKAL_TaskSleep(100U);
    }

    (void) AmbaKAL_TaskSleep(100);

    /* Update link status of other VIN which has the same Deserializer */
    for (i = 0; i < MAX96712_MAX_NUM; i++) {
        if ((i != VinID) && (IdcAddrMax96712[i] == pCurSerDesInfo->DesAddr)) {
            MaxSerDesInfo[i].EnabledLinkID |= pCurSerDesInfo->EnabledLinkID;
        }
    }
    pCurSerDesInfo->InitDone = 1U;

    return RetVal;
}

static UINT32 Max96717_96712_Config_Ser(UINT32 VinID, const MAX96717_96712_SERDES_CONFIG_s *pSerdesConfig)
{
    UINT32 RetVal = MAX96717_96712_ERR_NONE;
    UINT16 EnabledLinkID, i;
    UINT8 SerBroadcast;
    UINT8 SerAddr;
    UINT8 Data;
    UINT8 Index = 0U; //array index for NumDataBits, 0/1/2/3/4: 8/10/12/14/16 bit
    const UINT8 Reg0x0312[5] = {0x04U, 0x00U, 0x00U, 0x00U, 0x00U}; //RAW8/10/12/14/16, fix OX03C unexpected sync issue
    const UINT8 Reg0x0313[5] = {0x00U, 0x04U, 0x40U, 0x00U, 0x00U}; //RAW8/10/12/14/16
    const UINT8 SReg0x031E[5] = {0x30U, 0x34U, 0x38U, 0x00U, 0x00U}; //RAW8/10/12/14/16
    const UINT8 CurSerAddr[MAX96712_NUM_RX_PORT] = {MaxSerDesInfo[VinID].SerAddr0, MaxSerDesInfo[VinID].SerAddr1, MaxSerDesInfo[VinID].SerAddr2, MaxSerDesInfo[VinID].SerAddr3};

    if (pSerdesConfig == NULL) {
        RetVal = MAX96717_96712_ERR_ARG;
    } else {
        // Init assignment
        EnabledLinkID = pSerdesConfig->EnabledLinkID;

#ifdef SERIALIZER_BC
        AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] Serializer Broadcast I2C slave address: 0x%x", MX07_IDC_ADDR_MAX96717_BCST, 0U, 0U, 0U, 0U);
        SerBroadcast = 1U;

        for (i = 0U; i < MAX96712_NUM_RX_PORT; i ++) {
            if (((1U << i) & (UINT8)EnabledLinkID) != 0U) {
                RetVal |= Max96717_96712_RegWrite(VinID, CurSerAddr[i], 0x0042U, MX07_IDC_ADDR_MAX96717_BCST);
                RetVal |= Max96717_96712_RegWrite(VinID, CurSerAddr[i], 0x0043U, CurSerAddr[i]);
            }
        }
#else
        AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "[MAX96717_96712] Set-up serializer(s) in order", 0U, 0U, 0U, 0U, 0U);
        SerBroadcast = 0U;

#endif

        for (i = 0U; i < MAX96712_NUM_RX_PORT; i ++) {
            if (((1U << i) & (UINT8)EnabledLinkID) != 0U) {
#ifdef SERIALIZER_BC
                SerAddr = MX07_IDC_ADDR_MAX96717_BCST;
#else
                SerAddr = CurSerAddr[i];
#endif
                Index = (pSerdesConfig->NumDataBits - 8U) / 2U;

                // --------------------------------------------------------------------------------------
                // MAX96717 powers up in GMSL2 mode
                // Config MIPI PHY / Controller
                // Set SER in 1x4 mode; 4-lane in controller 1 for port A
                // ----------------------------------------------------------------------------------------
                AmbaPrint_ModulePrintUInt5(MAX96717_96712_MODULE_ID, "********   Link(%d) : Serializer MAX96717    ********", (UINT32)i + 1U, 0U, 0U, 0U, 0U);
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
                Data = (UINT8)((pSerdesConfig->CSIRxLaneNum - 1U) << 4U); // select number of data lanes
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);

                /* Serializer lane mapping for MIPI data lane 2 and 3 */
                Data = (UINT8)((pSerdesConfig->CSIRxLaneMapping[3] << 6U) | (pSerdesConfig->CSIRxLaneMapping[2] << 4U));
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0332U, Data); // IMX728: 0xEE, IMX623: 0x70
                /* Serializer lane mapping for MIPI data lane 0 and 1 */
                Data = (UINT8)((pSerdesConfig->CSIRxLaneMapping[1] << 2U) | pSerdesConfig->CSIRxLaneMapping[0]);
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0333U, Data); // IMX728: 0x01, IMX623: 0x02

                /* Serializer lane polarity for MIPI data lane 2 and 3 */
                Data = (UINT8)((pSerdesConfig->CSIRxLanePolarity[3] << 5U) | (pSerdesConfig->CSIRxLanePolarity[2] << 4U));
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0334U, Data); // IMX728: 0x00, IMX623: 0x30
                /* Serializer lane polarity for MIPI data lane 0 and 1 */
                Data = (UINT8)((pSerdesConfig->CSIRxLanePolarity[1] << 1U) | pSerdesConfig->CSIRxLanePolarity[0]);
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0335U, Data); // IMX728: 0x00, IMX623: 0x01

                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x64U); /* Select CSI port B for Video Pipe Z */
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x40U); /* Start Video Pipe Z from CSI port B */

                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0312U, Reg0x0312[Index]); // BPP 8 Double
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0313U, Reg0x0313[Index]); // BPP 10 Double BPP 12 Double

                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x43U);
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x005BU, 0x02U); /* Set Video Pipe Z Stream ID = 2 */
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0110U, 0x68U);
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0111U, 0x58U);
                Data = 0x40U | pSerdesConfig->DataType;
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0318U, Data); /*  RAW to pipe Z */
                RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x031EU, SReg0x031E[Index]); /* Software override enable for Video Pipe Z */

                /* Only set up once for the broadcast case */
                if (SerBroadcast == 1U) {
                    break;
                }
            }
        }

        // -----------------------------------------------------------
        //  Sensor I2C translation
        // -----------------------------------------------------------
        for (i = 0U; i < MAX96712_NUM_RX_PORT; i ++) {
            if (((1U << i) & (UINT8)EnabledLinkID) != 0U) {
                RetVal |= Max96717_96712_RegWrite(VinID, CurSerAddr[i], 0x0042U, pSerdesConfig->SensorAliasID[i]);
                RetVal |= Max96717_96712_RegWrite(VinID, CurSerAddr[i], 0x0043U, pSerdesConfig->SensorSlaveID);
                RetVal |= Max96717_96712_RegWrite(VinID, CurSerAddr[i], 0x0044U, pSerdesConfig->SensorBCID);
                RetVal |= Max96717_96712_RegWrite(VinID, CurSerAddr[i], 0x0045U, pSerdesConfig->SensorSlaveID);
            }
        }

    }
    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96717_96712_Config_Des
 *
 *  @Description:: Configure Max96717_96712 Deserializer
 *
 *  @Input      ::
 *      VinID:          VIN ID
 *      pSerdesConfig:   pointer to Serdes configuation
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 Max96717_96712_Config_Des(UINT32 VinID, const MAX96717_96712_SERDES_CONFIG_s *pSerdesConfig)
{
    const MAX96717_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX96717_96712_ERR_NONE;
    UINT16 EnabledLinkID;
    UINT8 DesAddr;
    UINT8 Data;
    UINT8 Index = 0U; //array index for NumDataBits, 0/1/2/3/4: 8/10/12/14/16 bit
    const UINT8 Reg0x0973[5] = {0x02U, 0x04U, 0x01U, 0x00U, 0x00U}; //8/10/12/14/16 bit, fix OX03C unexpected sync issue

    if (pSerdesConfig == NULL) {
        RetVal = MAX96717_96712_ERR_ARG;
    } else {
        //Init assignment
        EnabledLinkID = pSerdesConfig->EnabledLinkID;
        DesAddr = pCurSerDesInfo->DesAddr;
        Index = (pSerdesConfig->NumDataBits- 8U) / 2U; //TBD, will check how to set for different bit for each Link

        /* Config DES MAX96712 */
        // -----------------------------------------------------------
        // Deserializer MIPI Setup
        // -----------------------------------------------------------
        AmbaPrint_ModulePrintStr5(MAX96717_96712_MODULE_ID, "********    Derserialzier MAX96712    ********", NULL, NULL, NULL, NULL, NULL);


        // if (pSerdesConfig->VideoAggregationMode == MAX96717_MAX96712_VIDEO_AGGR_FCFS) {
        // Select Pipe Z in Link A to Video Pipe 0, Select Pipe Z in Link B to Video Pipe 1
        // Select Pipe Z in Link C to Video Pipe 2, Select Pipe Z in Link D to Video Pipe 3
        // Select Pipe Z in Link A to Video Pipe 4, Select Pipe Z in Link B to Video Pipe 5
        // Select Pipe Z in Link C to Video Pipe 6, Select Pipe Z in Link D to Video Pipe 7
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x00F0U, 0x62U);
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x00F1U, 0xEAU);
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x00F2U, 0x62U);
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x00F3U, 0xEAU);


        Data = EnabledLinkID;
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x00F4U, Data);

        // MIPI PHY Setting
        // Set Des in 2x4 mode
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08A0U, 0x04U); // MIPI_PHY:MIPI_PHY0
        // Set Lane Mapping for 4-lane port A
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08A3U, 0xE4U);
        // RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08A4U, 0xE4U);
        // Set D-PHY lane num
        // RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x090AU, ((pSerdesConfig->CSITxLaneNum[0]-1U) << 6U));
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x094AU, ((pSerdesConfig->CSITxLaneNum[0]-1U) << 6U));
        // RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x098AU, ((pSerdesConfig->CSITxLaneNum[0]-1U) << 6U));
        // RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09CAU, ((pSerdesConfig->CSITxLaneNum[0]-1U) << 6U));
        // Turn on MIPI PHYs PHY0 and PHY1
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08A2U, 0x30U); // MIPI_PHY:MIPI_PHY2, [7:4] = 0b1111 (MIPI PHY 0-1-2-3 Enable)
        // Put DPLL in reset while changing MIPI data rate
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x1D00U, 0xF4U);
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x1E00U, 0xF4U);

        // CSI bit rate per lane, CSITxSpeed * 100 MHz
        Data = 0x20U | pSerdesConfig->CSITxSpeed[0];
        // RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0415U, Data);
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0418U, Data);
        // RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x041BU, Data);
        // RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x041EU, Data);

        // release reset to DPLL1 and DPLL2
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x1D00U, 0xF5U);
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x1E00U, 0xF5U);

        // if (pSerdesConfig->VideoAggregationMode == MAX96717_MAX96712_VIDEO_AGGR_FCFS) {
        // LinkA
        if ((EnabledLinkID & 0x1U) != 0U) {
            // video pipe 0
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x090BU, 0x07U); // mapping enable (low byte)
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x092DU, 0x15U); // map to MIPI Controller 1
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x090DU, pSerdesConfig->DataType);
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x090EU, pSerdesConfig->DataType); // map data to VC0
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x090FU, 0x00U);
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0910U, 0x00U); // map FS to VC0
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0911U, 0x01U);
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0912U, 0x01U); // map FE to VC0

        }

        // LinkB
        if ((EnabledLinkID & 0x2U) != 0U) {
            // video pipe 1
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x094BU, 0x07U); // mapping enable (low byte)
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x096DU, 0x15U); // map to MIPI Controller 1
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x094DU, pSerdesConfig->DataType);
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x094EU, pSerdesConfig->DataType | (0x1U << 6U)); // map data to VC1
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x094FU, 0x00U);
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0950U, 0x40U); // map FS to VC1
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0951U, 0x01U);
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0952U, 0x41U); // map FE to VC1
        }

        // LinkC
        if ((EnabledLinkID & 0x4U) != 0U) {
            // video pipe 2
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x098BU, 0x07U); // mapping enable (low byte)
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09ADU, 0x15U); // map to MIPI Controller 1
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x098DU, pSerdesConfig->DataType);
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x098EU, pSerdesConfig->DataType | (0x2U << 6U)); // map data to VC2
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x098FU, 0x00U);
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0990U, 0x80U); // map FS to VC2
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0991U, 0x01U);
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0992U, 0x81U); // map FE to VC2
        }

        // LinkD
        if ((EnabledLinkID & 0x8U) != 0U) {
            // video pipe 3
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09CBU, 0x07U); // mapping enable (low byte)
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09EDU, 0x15U); // map to MIPI Controller 1
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09CDU, pSerdesConfig->DataType);
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09CEU, pSerdesConfig->DataType | (0x3U << 6U)); // map data to VC3
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09CFU, 0x00U);
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D0U, 0xC0U); // map FS to VC3
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D1U, 0x01U);
            RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D2U, 0xC1U); // map FE to VC3
        }

        //important !!! Depend on different silicon to disable FS/FE output.
        //ES1. Reg 0x259/0x279/0x299/0x2B9 = 0x39 (bit 5 = 1, force VS = 0) for pipe 4/5/6/7
        //ES2. Reg 0x436 (bit 4/5/6/7 = 1, force VS = 0) for pipe 4/5/6/7
        Data = (UINT8)(EnabledLinkID << 4U);
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0436U, Data);
        // }

        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0973U, Reg0x0973[Index]);
        // VS debug signal Output on MFP4
        RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x00FAU, 0x10U);

    }

    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96712_Config
 *
 *  @Description:: Configure Max96717_96712
 *
 *  @Input      ::
 *      VinID:          VIN ID
 *      pSerdesConfig:   pointer to Serdes configuation
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96717_96712_Config(UINT32 VinID, const MAX96717_96712_SERDES_CONFIG_s *pSerdesConfig)
{
    UINT32 RetVal = MAX96717_96712_ERR_NONE;

    AmbaPrint_PrintUInt5( ">>>>>[Max96717_96712_Config] Config (VinID: 0x%02x )", VinID, 0U, 0U, 0U, 0U);
    if (pSerdesConfig == NULL) {
        RetVal = MAX96717_96712_ERR_ARG;
    } else {

        /* default setting generated from MAXIM tool */
        RetVal |= Max96717_96712_SetDefaultConfig(VinID, pSerdesConfig);

        RetVal |= Max96717_96712_Config_Ser(VinID, pSerdesConfig);
        RetVal |= Max96717_96712_Config_Des(VinID, pSerdesConfig);

    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96712_SetSensorClk
 *
 *  @Description:: Set MAX96717 reference clock output for image sensor
 *
 *  @Input      ::
 *      VinID:          VIN ID
 *      ChipID:         Chip ID
 *      RClkGpio:       Selected MFP pin for RCLK output
 *      RClkOutFreq:    Reference clock output to selected MFP
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/

UINT32 Max96717_96712_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkGpio, UINT32 RClkOutFreq)
{
    UINT32 RetVal = MAX96717_96712_ERR_NONE;
    UINT8 SlaveID = 0x0U, Reg3Val;
    MAX96717_REG3_RCLK_s REG3 = {0};
    const MAX96717_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX96717_96712_ERR_INVALID_API;
    } else {
        if (ChipID == MAX96717_A_ID) {
            SlaveID = pCurSerDesInfo->SerAddr0;
        } else if (ChipID == MAX96717_B_ID) {
            SlaveID = pCurSerDesInfo->SerAddr1;
        } else if (ChipID == MAX96717_C_ID) {
            SlaveID = pCurSerDesInfo->SerAddr2;
        } else if (ChipID == MAX96717_D_ID) {
            SlaveID = pCurSerDesInfo->SerAddr3;
        } else {
            RetVal = MAX96717_96712_ERR_INVALID_API;
        }

        if (RetVal == MAX96717_96712_ERR_NONE) {

            RetVal |= Max96717_96712_RegRead(VinID, SlaveID, 0x0003, &Reg3Val);
            if (RetVal == MAX96717_96712_ERR_NONE) {
                (void) AmbaWrap_memcpy(&REG3, &Reg3Val, sizeof(Reg3Val));

                if (RClkOutFreq == MAX96717_96712_RCLK_OUT_FREQ_24M) {
                    // Settings generated by GMSL SerDes GUI V5.4.0 DPLL Tool
                    /* pre-defined clock setting is disabled */
                    RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x02, 0x43);
                    RetVal |= Max96717_96712_RegWrite(VinID, SlaveID, 0x03F4, 0x0A);
                    RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F5, 0x07, 0x0F);
                    RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x00, 0x03);
                    RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A03, 0x10, 0x90);
                    RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A07, 0x04, 0xFC);
                    RetVal |= Max96717_96712_RegWrite(VinID, SlaveID, 0x1A08, 0x3D);
                    RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A09, 0x40, 0xFF);
                    RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A0A, 0x40, 0x7F);
                    RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A0B, 0x60, 0xE0);
                    RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x01, 0x01);

                    REG3.RClkSel = 0x3U; /* Reference PLL output */

                } else if (RClkOutFreq == MAX96717_96712_RCLK_OUT_FREQ_25M) {
                    REG3.RClkSel = 0x0U; /* RCLKOUT XTAL/1 = 25MHz */

                } else {
                    RetVal = MAX96717_96712_ERR_ARG;
                }

                RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x0006, 0x20, 0x20); /* Enable RCLK output */

                /* Support routing RCLK to MFP2 and MFP4 only */
                if (RClkGpio == MAX96717_GPIO_PIN_2) { /* Output RCLK_ALT (MFP #2) */
                    RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x056F, 0x00, 0x30); /* Set fastest slew rate for MFP2 pin */
                    REG3.RClkAlt = 0x1U; /* Route RCLK to MFP2 */

                } else if (RClkGpio == MAX96717_GPIO_PIN_4) { /* Output RCLK (MFP #4) Default */
                    RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x0570, 0x00, 0x30); /* Set fastest slew rate for MFP4 pin */
                    REG3.RClkAlt = 0x0U; /* Route RCLK to MFP4 */

                } else {
                    RetVal = MAX96717_96712_ERR_ARG;
                }

                (void) AmbaWrap_memcpy(&Reg3Val, &REG3, sizeof(Reg3Val));
                RetVal |= Max96717_96712_RegReadModifyWrite(VinID, SlaveID, 0x0003, Reg3Val, 0x07);
            }
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96712_SetGpioOutput
 *
 *  @Description:: Set MAX96717 reference clock output for image sensor
 *
 *  @Input      ::
 *      VinID:     VIN ID
 *      ChipID:    Chip ID
 *      PinID:     Gpio pin ID
 *      Level:     Gpio level
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96717_96712_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT8 Level)
{
    UINT32 RetVal = MAX96717_96712_ERR_NONE;
    UINT16 Addr = 0U; /* register address */
    MAX96717_96712_GPIO_A_REG_s GpioAReg = {0};
    MAX96717_96712_GPIO_B_REG_s GpioBReg = {0};
    const MAX96717_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT8 SlaveID = 0U, RegAVal, RegBVal;

    if (ChipID == MAX96712_ID) {
        if (PinID >= MAX96712_NUM_GPIO_PIN) {
            RetVal = MAX96717_96712_ERR_ARG;
        } else {
            Addr = (UINT16)0x2B0 + (UINT16)(PinID * 3U);
            SlaveID = pCurSerDesInfo->DesAddr;
        }
    } else {
        if (PinID >= MAX96717_NUM_GPIO_PIN) {
            RetVal = MAX96717_96712_ERR_ARG;
        } else {
            if (ChipID == MAX96717_A_ID) {
                Addr = (UINT16)0x2BE + (UINT16)(PinID * 3U);
                SlaveID = pCurSerDesInfo->SerAddr0;
            } else if (ChipID == MAX96717_B_ID) {
                Addr = (UINT16)0x2BE + (UINT16)(PinID * 3U);
                SlaveID = pCurSerDesInfo->SerAddr1;
            } else if (ChipID == MAX96717_C_ID) {
                Addr = (UINT16)0x2BE + (UINT16)(PinID * 3U);
                SlaveID = pCurSerDesInfo->SerAddr2;
            } else if (ChipID == MAX96717_D_ID) {
                Addr = (UINT16)0x2BE + (UINT16)(PinID * 3U);
                SlaveID = pCurSerDesInfo->SerAddr3;
            } else {
                RetVal = MAX96717_96712_ERR_ARG;
            }
        }
    }

    if (RetVal == MAX96717_96712_ERR_NONE) {
        RetVal |= Max96717_96712_RegRead(VinID, SlaveID, Addr, &RegAVal);
        RetVal |= Max96717_96712_RegRead(VinID, SlaveID, Addr + 1U, &RegBVal);
        if (RetVal == MAX96717_96712_ERR_NONE) {
            (void) AmbaWrap_memcpy(&GpioAReg, &RegAVal, sizeof(RegAVal));
            (void) AmbaWrap_memcpy(&GpioBReg, &RegBVal, sizeof(RegBVal));

            GpioBReg.GpioTxId = (UINT8)PinID;
            GpioBReg.OutType = 1U;
            GpioBReg.PullUpDnSel = 0U;

            GpioAReg.GpioOutDis = 0U;
            GpioAReg.GpioTxEn = 0U;
            GpioAReg.GpioRxEn = 0U;
            GpioAReg.GpioIn = 0U;
            if (Level == 0U) {
                GpioAReg.GpioOut = 0U;
            } else {
                GpioAReg.GpioOut = 1U;
            }
            GpioAReg.TxCompEn = 0U;
            GpioAReg.TxPrio = 0U;
            GpioAReg.ResCfg = 1U;

            (void) AmbaWrap_memcpy(&RegAVal, &GpioAReg, sizeof(RegAVal));
            (void) AmbaWrap_memcpy(&RegBVal, &GpioBReg, sizeof(RegBVal));
            RetVal |= Max96717_96712_RegWrite(VinID, SlaveID, Addr, RegAVal);
            RetVal |= Max96717_96712_RegWrite(VinID, SlaveID, Addr + 1U, RegBVal);
        } else {
            RetVal = MAX96717_96712_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96712_GPIO_Forwarding
 *
 *  @Description:: Configure GMSL2 GPIO Forwarding
 *
 *  @Input      ::
 *     VinID:           VIN ID
 *     EnabledLinkID:   Enabled LinkID on MAX96712
 *     pGpioFwding:     Pointer to GPIO Forwarding configurations
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96717_96712_GPIO_Forwarding(UINT32 VinID, UINT16 EnabledLinkID, const MAX96717_96712_GPIO_FORWARDING_CONFIG_s *pGpioFwding)
{
    UINT32 RetVal = MAX96717_96712_ERR_NONE;
    UINT16 SerRegAddr = 0U, DesRegAddr = 0U, DesMRegAddr = 0U;
    UINT8 SerSlaveID = 0U, DesSlaveID = 0U, RegAVal, RegBVal, RegCVal, RegMVal, i = 0U;
    MAX96717_96712_GPIO_A_REG_s GPIO_A_Reg = {0};
    MAX96717_96712_GPIO_B_REG_s GPIO_B_Reg = {0};
    MAX96717_96712_GPIO_C_REG_s GPIO_C_Reg = {0};
    MAX96717_96712_GPIO_MULTI_REG_s GPIO_M_Reg = {0}; // Config MAX96712 MFP pin to receive external FSYNC signal for each link
    const MAX96717_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if ((pGpioFwding == NULL)||
        (pGpioFwding->SerGpio >= MAX96717_NUM_GPIO_PIN)||
        (pGpioFwding->DesGpio >= MAX96712_NUM_GPIO_PIN)) {
        RetVal = MAX96717_96712_ERR_ARG;
    } else {
        SerRegAddr = MAX96717_GPIO_BASE_ADDR + ((UINT16)pGpioFwding->SerGpio * 3U);
        DesRegAddr = MAX96712_GPIO_BASE_ADDR + ((UINT16)pGpioFwding->DesGpio * 3U);
        DesMRegAddr = MAX96712_M_GPIO_BASE_ADDR + ((UINT16)pGpioFwding->DesGpio * 3U);

        // For Multi Chan, Addr 0x***1 => +4 offset compared to previous one while +3 for others B(MFP3_8_13),C(MFP1_6_11_16),D(MFP4_9_14)
        if ((EnabledLinkID & MAX96712_LINK_A) != 0U) {
            SerSlaveID = pCurSerDesInfo->SerAddr0;
        } else if ((EnabledLinkID & MAX96712_LINK_B) != 0U) {
            SerSlaveID = pCurSerDesInfo->SerAddr1;
            // B(MFP3_8_13)
            if (pGpioFwding->DesGpio > 12U) {
                DesMRegAddr += 3U;
            } else if (pGpioFwding->DesGpio > 7U) {
                DesMRegAddr += 2U;
            } else if (pGpioFwding->DesGpio > 2U) {
                DesMRegAddr += 1U;
            } else {
                // not need to modify offset
            }
        } else if ((EnabledLinkID & MAX96712_LINK_C) != 0U) {
            SerSlaveID = pCurSerDesInfo->SerAddr2;
            DesMRegAddr += ((17U * 3U) + 3U); // MFP0-16 with A/B/C and B(MFP3_8_13)
            // C(MFP1_6_11_16)
            if (pGpioFwding->DesGpio > 15U) {
                DesMRegAddr += 4U;
            } else if (pGpioFwding->DesGpio > 10U) {
                DesMRegAddr += 3U;
            } else if (pGpioFwding->DesGpio > 5U) {
                DesMRegAddr += 2U;
            } else if (pGpioFwding->DesGpio > 0U) {
                DesMRegAddr += 1U;
            } else {
                // not need to modify offset
            }
        } else if ((EnabledLinkID & MAX96712_LINK_D) != 0U) {
            SerSlaveID = pCurSerDesInfo->SerAddr3;
            DesMRegAddr += (((17U * 3U) * 2U) + 3U + 4U); // MFP0-16 with A/B/C and B(MFP3_8_13),C(MFP1_6_11_16)
            // D(MFP4_9_14)
            if (pGpioFwding->DesGpio > 13U) {
                DesMRegAddr += 3U;
            } else if (pGpioFwding->DesGpio > 8U) {
                DesMRegAddr += 2U;
            } else if (pGpioFwding->DesGpio > 3U) {
                DesMRegAddr += 1U;
            } else {
                // not need to modify offset
            }
        } else {
            RetVal = MAX96717_96712_ERR_SERDES_LINK;
        }

        DesSlaveID = pCurSerDesInfo->DesAddr;

        if (RetVal == MAX96717_96712_ERR_NONE) {
            if (pGpioFwding->Direction == MAX96717_96712_GPIO_DIR_SER2DES) {
                /* Config Serializer to TX */
                RetVal |= Max96717_96712_RegRead(VinID, SerSlaveID, SerRegAddr, &RegAVal);
                RetVal |= Max96717_96712_RegRead(VinID, SerSlaveID, SerRegAddr + 1U, &RegBVal);

                if (RetVal == MAX96717_96712_ERR_NONE) {
                    (void) AmbaWrap_memcpy(&GPIO_A_Reg, &RegAVal, sizeof(RegAVal));
                    (void) AmbaWrap_memcpy(&GPIO_B_Reg, &RegBVal, sizeof(RegBVal));
                    GPIO_A_Reg.TxCompEn = 1U;
                    GPIO_A_Reg.TxPrio = pGpioFwding->Priority;
                    GPIO_A_Reg.ResCfg = pGpioFwding->ResistorCfg;

                    GPIO_B_Reg.OutType = pGpioFwding->OutType;
                    GPIO_B_Reg.PullUpDnSel = pGpioFwding->PullUpDnSel;

                    GPIO_A_Reg.GpioOutDis = 1U;
                    GPIO_A_Reg.GpioTxEn = 1U;
                    GPIO_A_Reg.GpioRxEn = 0U;
                    GPIO_B_Reg.GpioTxId = pGpioFwding->SerGpio;

                    (void) AmbaWrap_memcpy(&RegAVal, &GPIO_A_Reg, sizeof(RegAVal));
                    (void) AmbaWrap_memcpy(&RegBVal, &GPIO_B_Reg, sizeof(RegBVal));
                    RetVal |= Max96717_96712_RegWrite(VinID, SerSlaveID, SerRegAddr, RegAVal);
                    RetVal |= Max96717_96712_RegWrite(VinID, SerSlaveID, SerRegAddr + 1U, RegBVal);
                } else {
                    RetVal = MAX96717_96712_ERR_COMMUNICATE;
                }

                /* Config Deserializer to RX */
                RetVal |= Max96717_96712_RegRead(VinID, DesSlaveID, DesRegAddr, &RegAVal);
                RetVal |= Max96717_96712_RegRead(VinID, DesSlaveID, DesRegAddr + 1U, &RegBVal);
                RetVal |= Max96717_96712_RegRead(VinID, DesSlaveID, DesRegAddr + 2U, &RegCVal);

                if (RetVal == MAX96717_96712_ERR_NONE) {
                    (void) AmbaWrap_memcpy(&GPIO_A_Reg, &RegAVal, sizeof(RegAVal));
                    (void) AmbaWrap_memcpy(&GPIO_B_Reg, &RegBVal, sizeof(RegBVal));
                    (void) AmbaWrap_memcpy(&GPIO_C_Reg, &RegCVal, sizeof(RegCVal));
                    GPIO_A_Reg.TxCompEn = 1U;
                    GPIO_A_Reg.TxPrio = pGpioFwding->Priority;
                    GPIO_A_Reg.ResCfg = pGpioFwding->ResistorCfg;

                    GPIO_B_Reg.OutType = pGpioFwding->OutType;
                    GPIO_B_Reg.PullUpDnSel = pGpioFwding->PullUpDnSel;

                    /* Setup RX */
                    GPIO_A_Reg.GpioOutDis = 0U;
                    GPIO_A_Reg.GpioTxEn = 0U;
                    GPIO_A_Reg.GpioRxEn = 1U;
                    GPIO_C_Reg.GpioRxId = pGpioFwding->SerGpio; // Same as SER TxId

                    (void) AmbaWrap_memcpy(&RegAVal, &GPIO_A_Reg, sizeof(RegAVal));
                    (void) AmbaWrap_memcpy(&RegBVal, &GPIO_B_Reg, sizeof(RegBVal));
                    (void) AmbaWrap_memcpy(&RegCVal, &GPIO_C_Reg, sizeof(RegCVal));
                    RetVal |= Max96717_96712_RegWrite(VinID, DesSlaveID, DesRegAddr, RegAVal);
                    RetVal |= Max96717_96712_RegWrite(VinID, DesSlaveID, DesRegAddr + 1U, RegBVal);
                    RetVal |= Max96717_96712_RegWrite(VinID, DesSlaveID, DesRegAddr + 2U, RegCVal);
                } else {
                    RetVal = MAX96717_96712_ERR_COMMUNICATE;
                }

            } else if (pGpioFwding->Direction == MAX96717_96712_GPIO_DIR_DES2SER) {
                /* Config Deserializer to TX */
                RetVal |= Max96717_96712_RegRead(VinID, DesSlaveID, DesRegAddr, &RegAVal);
                RetVal |= Max96717_96712_RegRead(VinID, DesSlaveID, DesRegAddr + 1U, &RegBVal);

                if (RetVal == MAX96717_96712_ERR_NONE) {
                    (void) AmbaWrap_memcpy(&GPIO_A_Reg, &RegAVal, sizeof(RegAVal));
                    (void) AmbaWrap_memcpy(&GPIO_B_Reg, &RegBVal, sizeof(RegBVal));
                    GPIO_A_Reg.TxCompEn = 1U;
                    GPIO_A_Reg.TxPrio = pGpioFwding->Priority;
                    GPIO_A_Reg.ResCfg = pGpioFwding->ResistorCfg;

                    GPIO_B_Reg.OutType = pGpioFwding->OutType;
                    GPIO_B_Reg.PullUpDnSel = pGpioFwding->PullUpDnSel;

                    /* Setup TX */
                    GPIO_A_Reg.GpioOutDis = 1U;
                    GPIO_A_Reg.GpioTxEn = 1U;
                    GPIO_A_Reg.GpioRxEn = 0U;
                    GPIO_B_Reg.GpioTxId = pGpioFwding->DesGpio;

                    (void) AmbaWrap_memcpy(&RegAVal, &GPIO_A_Reg, sizeof(RegAVal));
                    (void) AmbaWrap_memcpy(&RegBVal, &GPIO_B_Reg, sizeof(RegBVal));
                    RetVal |= Max96717_96712_RegWrite(VinID, DesSlaveID, DesRegAddr, RegAVal);
                    RetVal |= Max96717_96712_RegWrite(VinID, DesSlaveID, DesRegAddr + 1U, RegBVal);

                    // Set Internal FSYNC off, GPIO is used for FSYNC, type = GMSL2
                    RetVal |= Max96717_96712_RegWrite(VinID, DesSlaveID, 0x04A0U, 0x08U);
                    RetVal |= Max96717_96712_RegWrite(VinID, DesSlaveID, 0x04AFU, 0x9FU);
                    for (i = 1U; i < MAX96712_NUM_RX_PORT; i ++) {
                        if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
                            // Config MAX96712 MFP pin to receive external FSYNC signal for each link
                            RetVal |= Max96717_96712_RegRead(VinID, DesSlaveID, DesMRegAddr, &RegMVal);
                            (void) AmbaWrap_memcpy(&GPIO_M_Reg, &RegMVal, sizeof(RegMVal));
                            if (RetVal == MAX96717_96712_ERR_NONE) {
                                /* Setup TX */
                                GPIO_M_Reg.TxCompEn = 1U;
                                GPIO_M_Reg.TxPrio = pGpioFwding->Priority;
                                GPIO_M_Reg.GpioTxEn = 1U;
                                GPIO_M_Reg.GpioTxId = pGpioFwding->DesGpio;
                                (void) AmbaWrap_memcpy(&RegMVal, &GPIO_M_Reg, sizeof(RegMVal));
                                RetVal |= Max96717_96712_RegWrite(VinID, DesSlaveID, DesMRegAddr, RegMVal);
                            }
                        }
                    }
                } else {
                    RetVal = MAX96717_96712_ERR_COMMUNICATE;
                }

                /* Config Serializer to RX */
                RetVal |= Max96717_96712_RegRead(VinID, SerSlaveID, SerRegAddr, &RegAVal);
                RetVal |= Max96717_96712_RegRead(VinID, SerSlaveID, SerRegAddr + 1U, &RegBVal);
                RetVal |= Max96717_96712_RegRead(VinID, SerSlaveID, SerRegAddr + 2U, &RegCVal);

                if (RetVal == MAX96717_96712_ERR_NONE) {
                    (void) AmbaWrap_memcpy(&GPIO_A_Reg, &RegAVal, sizeof(RegAVal));
                    (void) AmbaWrap_memcpy(&GPIO_B_Reg, &RegBVal, sizeof(RegBVal));
                    (void) AmbaWrap_memcpy(&GPIO_C_Reg, &RegCVal, sizeof(RegCVal));
                    GPIO_A_Reg.TxCompEn = 1U;
                    GPIO_A_Reg.TxPrio = pGpioFwding->Priority;
                    GPIO_A_Reg.ResCfg = pGpioFwding->ResistorCfg;

                    GPIO_B_Reg.OutType = pGpioFwding->OutType;
                    GPIO_B_Reg.PullUpDnSel = pGpioFwding->PullUpDnSel;

                    /* Setup RX */
                    GPIO_A_Reg.GpioOutDis = 0U;
                    GPIO_A_Reg.GpioTxEn = 0U;
                    GPIO_A_Reg.GpioRxEn = 1U;
                    GPIO_C_Reg.GpioRxId = pGpioFwding->DesGpio; // Same as DES TxId

                    (void) AmbaWrap_memcpy(&RegAVal, &GPIO_A_Reg, sizeof(RegAVal));
                    (void) AmbaWrap_memcpy(&RegBVal, &GPIO_B_Reg, sizeof(RegBVal));
                    (void) AmbaWrap_memcpy(&RegCVal, &GPIO_C_Reg, sizeof(RegCVal));
                    RetVal |= Max96717_96712_RegWrite(VinID, SerSlaveID, SerRegAddr, RegAVal);
                    RetVal |= Max96717_96712_RegWrite(VinID, SerSlaveID, SerRegAddr + 1U, RegBVal);
                    RetVal |= Max96717_96712_RegWrite(VinID, SerSlaveID, SerRegAddr + 2U, RegCVal);
                } else {
                    RetVal = MAX96717_96712_ERR_COMMUNICATE;
                }

            } else {
                RetVal = MAX96717_96712_ERR_ARG;
            }
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96712_RegisterWrite
 *
 *  @Description:: Write Max96717_96712 registers API
 *
 *  @Input      ::
 *      VinID:     VIN ID
 *      ChipID:    Chip ID
 *      Addr:      Register address
 *      Data:      Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96717_96712_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 Data)
{
    UINT32 RetVal = MAX96717_96712_ERR_NONE;
    const MAX96717_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX96717_96712_ERR_INVALID_API;
    } else {
        if (ChipID == MAX96712_ID) {
            RetVal = Max96717_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, Addr, Data);
        } else if (ChipID == MAX96717_A_ID) {
            RetVal = Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr0, Addr, Data);
        } else if (ChipID == MAX96717_B_ID) {
            RetVal = Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr1, Addr, Data);
        } else if (ChipID == MAX96717_C_ID) {
            RetVal = Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr2, Addr, Data);
        } else if (ChipID == MAX96717_D_ID) {
            RetVal = Max96717_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr3, Addr, Data);
        } else {
            RetVal = MAX96717_96712_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96712_RegisterRead
 *
 *  @Description:: Read Max96717_96712 registers API
 *
 *  @Input      ::
 *      VinID:     VIN ID
 *      Addr:      Register address
 *      pData:     Pointer to register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96717_96712_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 *pData)
{
    UINT32 RetVal = MAX96717_96712_ERR_NONE;
    const MAX96717_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX96717_96712_ERR_INVALID_API;
    } else {
        if (ChipID == MAX96712_ID) {
            RetVal = Max96717_96712_RegRead(VinID, pCurSerDesInfo->DesAddr, Addr, pData);
        } else if (ChipID == MAX96717_A_ID) {
            RetVal = Max96717_96712_RegRead(VinID, pCurSerDesInfo->SerAddr0, Addr, pData);
        } else if (ChipID == MAX96717_B_ID) {
            RetVal = Max96717_96712_RegRead(VinID, pCurSerDesInfo->SerAddr1, Addr, pData);
        } else if (ChipID == MAX96717_C_ID) {
            RetVal = Max96717_96712_RegRead(VinID, pCurSerDesInfo->SerAddr2, Addr, pData);
        } else if (ChipID == MAX96717_D_ID) {
            RetVal = Max96717_96712_RegRead(VinID, pCurSerDesInfo->SerAddr3, Addr, pData);
        } else {
            RetVal = MAX96717_96712_ERR_ARG;
        }
    }

    return RetVal;
}

static UINT32 Max96717_96712_SetDefaultConfig(UINT32 VinID, const MAX96717_96712_SERDES_CONFIG_s *pSerdesConfig)
{
    UINT32 RetVal = MAX96717_96712_ERR_NONE;
    UINT32 i;
    UINT8 SerAddr[MAX96712_NUM_RX_PORT] = {MaxSerDesInfo[VinID].SerAddr0, MaxSerDesInfo[VinID].SerAddr1, MaxSerDesInfo[VinID].SerAddr2, MaxSerDesInfo[VinID].SerAddr3};
    UINT8 DesAddr = MaxSerDesInfo[VinID].DesAddr;
    UINT8 Data;

    //*******************************************************************************
    // MAX96712 Setup
    //*******************************************************************************
    // pipe Z in link B to video pipe 1, pipe Z in link A to video pipe 0
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x00F0U, 0x12U);
    //; MAX96717A pipe Y in link A --> MAX96712 video pipe 4 and use PHY A (bit[3:0] = 0001'b)
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x00F1U, 0xEAU);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x00F2U, 0x40U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x00F3U, 0xC8U);
    //; Enable pipe 0 and pipe 1
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x00F4U, 0x03U);

    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0100U, 0x23U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0112U, 0x23U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0124U, 0x23U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0136U, 0x23U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0148U, 0x23U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0160U, 0x23U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0172U, 0x23U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0184U, 0x23U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0401U, 0x01U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0403U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0405U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0407U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x040BU, 0x02U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x040CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x040DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x040EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x040FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0410U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0411U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0412U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0414U, 0x00U);

    //Put DPLL in reset while changing MIPI data rate
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x1D00U, 0xF4U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x1E00U, 0xF4U);


    // CSI bit rate per lane, CSITxSpeed * 100 MHz
    Data = 0x20U | pSerdesConfig->CSITxSpeed[0];
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0415U, Data);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0417U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0418U, Data);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x041AU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x041BU, Data);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x041DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x041EU, Data);

    //release reset to DPLL1 and DPLL2
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x1D00U, 0xF5U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x1E00U, 0xF5U);

    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x041FU, 0x00U); // BACKTOP:BACKTOP32
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0421U, 0x00U); // BACKTOP_1:BACKTOP2
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0423U, 0x00U); // BACKTOP_1:BACKTOP4
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0425U, 0x00U); // BACKTOP_1:BACKTOP6
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0427U, 0x00U); // BACKTOP_1:BACKTOP8
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x042BU, 0x00U); // BACKTOP_1:BACKTOP12
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x042CU, 0x00U); // BACKTOP_1:BACKTOP13
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x042DU, 0x00U); // BACKTOP_1:BACKTOP14
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x042EU, 0x00U); // BACKTOP_1:BACKTOP15
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x042FU, 0x00U); // BACKTOP_1:BACKTOP16
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0430U, 0x00U); // BACKTOP_1:BACKTOP17
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0431U, 0x00U); // BACKTOP_1:BACKTOP18
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0432U, 0x00U); // BACKTOP_1:BACKTOP19
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0434U, 0x00U); // BACKTOP_1:BACKTOP21
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0435U, 0x01U); // BACKTOP_1:BACKTOP22  (default)
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0436U, 0x00U); // BACKTOP_1:BACKTOP23
                                                                   //   [3] = 0b0 (Pipe 3 VS disable)
                                                                   //   [2] = 0b0 (Pipe 2 VS disable)
                                                                   //   [1] = 0b0 (Pipe 1 VS disable)
                                                                   //   [0] = 0b0 (Pipe 0 VS disable)

    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0437U, 0x00U); // BACKTOP_1:BACKTOP24
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x043AU, 0x00U); // BACKTOP_1:BACKTOP27
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x043DU, 0x00U); // BACKTOP_1:BACKTOP30
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x043EU, 0x00U); // BACKTOP_1:BACKTOP31
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x043FU, 0x00U); // BACKTOP_1:BACKTOP32

    //MIPI PHY Setting
    //Set Des in 2x4 mode
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08A0U, 0x04U); // MIPI_PHY:MIPI_PHY0
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08A2U, 0xF0U); // MIPI_PHY:MIPI_PHY2
                                                                   //   [7:4] = 0b1111 (MIPI PHY 0-1-2-3 Enable)                                                                  //   [4:0] = 0b00100 (MIPI Output Configuration (2x4-4x2-etc))

    //Set Lane Mapping for 4-lane port A
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08A3U, 0xE4U); // MIPI_PHY:MIPI_PHY3
                                                                   //   [7:6] = 0b11 (set PHY1 data lane 1)
                                                                   //   [5:4] = 0b10 (set PHY1 data lane 0)
                                                                   //   [3:2] = 0b01 (set PHY0 data lane 1)
                                                                   //   [1:0] = 0b00 (set PHY0 data lane 0)
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08A4U, 0xE4U); // MIPI_PHY:MIPI_PHY4
                                                                   //   [7:6] = 0b11 (set PHY3 data lane 1)
                                                                   //   [5:4] = 0b10 (set PHY3 data lane 0)
                                                                   //   [3:2] = 0b01 (set PHY2 data lane 1)
                                                                   //   [1:0] = 0b00 (set PHY2 data lane 0)
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08A5U, 0x00U); // MIPI_PHY:MIPI_PHY5
                                                                   //   [5] = 0b0 (set polarity on PHY1 CLK lane)
                                                                   //   [4] = 0b0 (set polarity on PHY1 D1 lane)
                                                                   //   [3] = 0b0 (set polarity on PHY1 D0 lane)
                                                                   //   [1] = 0b0 (set polarity on PHY0 D1 lane)
                                                                   //   [0] = 0b0 (set polarity on PHY0 D0 lane)
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08A6U, 0x00U); // MIPI_PHY:MIPI_PHY6
                                                                   //   [4] = 0b0 (set polarity on PHY3 D1 lane)
                                                                   //   [3] = 0b0 (set polarity on PHY3 D0 lane)
                                                                   //   [2] = 0b0 (set polarity on PHY2 CLK lane)
                                                                   //   [1] = 0b0 (set polarity on PHY2 D1 lane)
                                                                   //   [0] = 0b0 (set polarity on PHY2 D0 lane)
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08A9U, 0x00U); // MIPI_PHY:MIPI_PHY9
                                                                   //   [7] = 0b0 (PHY copy 0 Enable)
                                                                   //   [6:5] = 0b00 (PHY copy 0 Destination)
                                                                   //   [4:3] = 0b00 (PHY copy 0 Source)
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08AAU, 0x02U); // MIPI_PHY:MIPI_PHY10
                                                                   //   [7] = 0b0 (PHY copy 1 Enable)
                                                                   //   [6:5] = 0b00 (PHY copy 1 Destination)
                                                                   //   [4:3] = 0b00 (PHY copy 1 Source)
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08ADU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x08AEU, 0x00U);

    //Set 4 lane D-PHY
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x090AU, 0x00U);
    //; Video Pipe to MIPI Controller Mapping
    //; RAW12, video pipe 0
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x090BU, 0x07U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x090CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x090DU, pSerdesConfig->DataType);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x090EU, pSerdesConfig->DataType);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x090FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0910U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0911U, 0x01U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0912U, 0x01U);

    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0913U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0914U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0915U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0916U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0917U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0918U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0919U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x091AU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x091BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x091CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x091DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x091EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x091FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0920U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0921U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0922U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0923U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0924U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0925U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0926U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0927U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0928U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0929U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x092AU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x092BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x092CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x092DU, 0x15U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x092EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x092FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0930U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0931U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0933U, 0x00U);

    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x094AU, 0xC0U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x094BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x094CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x094DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x094EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x094FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0950U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0951U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0952U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0953U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0954U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0955U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0956U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0957U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0958U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0959U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x095AU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x095BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x095CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x095DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x095EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x095FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0960U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0961U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0962U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0963U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0964U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0965U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0966U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0967U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0968U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0969U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x096AU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x096BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x096CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x096DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x096EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x096FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0970U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0971U, 0x00U);
//    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0973U, DReg0x0973[Index]);

    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x098AU, 0xC0U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x098BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x098CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x098DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x098EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x098FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0990U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0991U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0992U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0993U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0994U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0995U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0996U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0997U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0998U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0999U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x099AU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x099BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x099CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x099DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x099EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x099FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09A0U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09A1U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09A2U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09A3U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09A4U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09A5U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09A6U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09A7U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09A8U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09A9U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09AAU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09ABU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09ACU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09ADU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09AEU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09AFU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09B0U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09B1U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09B3U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09CAU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09CBU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09CCU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09CDU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09CEU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09CFU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D0U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D1U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D2U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D3U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D4U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D5U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D6U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D7U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D8U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09D9U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09DAU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09DBU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09DCU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09DDU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09DEU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09DFU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09E0U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09E1U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09E2U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09E3U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09E4U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09E5U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09E6U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09E7U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09E8U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09E9U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09EAU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09EBU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09ECU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09EDU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09EEU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09EFU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09F0U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09F1U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x09F3U, 0x00U);

    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A0BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A0CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A0DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A0EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A0FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A10U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A11U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A12U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A13U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A14U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A15U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A16U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A17U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A18U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A19U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A1AU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A1BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A1CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A1DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A1EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A1FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A20U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A21U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A22U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A23U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A24U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A25U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A26U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A27U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A28U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A29U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A2AU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A2BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A2CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A2DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A2EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A2FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A30U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A31U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A4BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A4CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A4DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A4EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A4FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A50U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A51U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A52U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A53U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A54U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A55U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A56U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A57U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A58U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A59U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A5AU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A5BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A5CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A5DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A5EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A5FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A60U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A61U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A62U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A63U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A64U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A65U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A66U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A67U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A68U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A69U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A6AU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A6BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A6CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A6DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A6EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A6FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A70U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A71U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A8BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A8CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A8DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A8EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A8FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A90U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A91U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A92U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A93U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A94U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A95U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A96U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A97U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A98U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A99U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A9AU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A9BU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A9CU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A9DU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A9EU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0A9FU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AA0U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AA1U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AA2U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AA3U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AA4U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AA5U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AA6U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AA7U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AA8U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AA9U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AAAU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AABU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AACU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AADU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AAEU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AAFU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AB0U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AB1U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0ACBU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0ACCU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0ACDU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0ACEU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0ACFU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AD0U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AD1U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AD2U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AD3U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AD4U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AD5U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AD6U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AD7U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AD8U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AD9U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0ADAU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0ADBU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0ADCU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0ADDU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0ADEU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0ADFU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AE0U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AE1U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AE2U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AE3U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AE4U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AE5U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AE6U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AE7U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AE8U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AE9U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AEAU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AEBU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AECU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AEDU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AEEU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AEFU, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AF0U, 0x00U);
    RetVal |= Max96717_96712_RegWrite(VinID, DesAddr, 0x0AF1U, 0x00U);
    for (i = 0U; i < MAX96712_NUM_RX_PORT; i ++) {
        if (((1U << i) & pSerdesConfig->EnabledLinkID) != 0U) {
            RetVal |= Max96717_96712_RegWrite(VinID, SerAddr[i], 0x0383, 0x00); // Disable tunneling mode
        }
    }
    //RetVal |= Max96717_96712_RegWrite(VinID, SerAddr, 0x0004, 0x1B); // Enable XTAL mode
    return RetVal;
}
