/**
 *  @file AmbaSbrg_Max96717_96716.c
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
 *  @details MAX96717 & MAX9296 driver APIs
 *
 */

#include <AmbaWrap.h>
#include "AmbaKAL.h"
#include "AmbaI2C.h"
#include "AmbaPrint.h"
#include "AmbaVIN.h"
#include "AmbaSbrg_Max96717_96716.h"

#include "bsp.h"

#define MAX96716_MAX_NUM        AMBA_NUM_VIN_CHANNEL

typedef struct {
    UINT32 InitDone;
    UINT8  DesAddr;          /* Max96716 I2C Slave Addr */
    UINT8  SerAddr0;         /* Distinguishable MAX96717 LinkA I2C Slave Addr */
    UINT8  SerAddr1;         /* Distinguishable MAX96717 LinkB I2C Slave Addr */
    UINT8  EnabledLinkID;
} MAX96717_96716_SERDES_INFO_s;

static UINT32 MAX96717_96716I2cChannel[MAX96716_MAX_NUM] = {
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

static MAX96717_96716_SERDES_INFO_s MaxSerDesInfo[MAX96716_MAX_NUM] = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96717_96716_RegWrite
 *
 *  @Description:: Write Max96717_96716 registers
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
static UINT32 Max96717_96716_RegWrite(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 Data)
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

    if (AmbaI2C_MasterWrite(MAX96717_96716I2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                            &I2cConfig, &TxSize, 1000U) != OK) {
        AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "SlaveID 0x%02x Addr 0x%04x I2C does not work!!!!!", SlaveID, Addr, 0U, 0U, 0U);
        RetVal = MAX96717_96716_ERR_COMMUNICATE;
    } else {
        AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "[MAX96717_96716 Ch%d W] %02X %02X %02X", MAX96717_96716I2cChannel[VinID], SlaveID, Addr, Data, 0U);
        RetVal = MAX96717_96716_ERR_NONE;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96717_96716_RegRead
 *
 *  @Description:: Read Max96717_96716 registers
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
static UINT32 Max96717_96716_RegRead(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 *pRxData)
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

    if (AmbaI2C_MasterReadAfterWrite(MAX96717_96716I2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                                     1U, &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U) != OK) {
        AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "SlaveID 0x%02x Addr 0x%04x I2C does not work!!!!!", SlaveID, Addr, 0U, 0U, 0U);
        RetVal = MAX96717_96716_ERR_COMMUNICATE;
    } else {
        // AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "[MAX96717_96716 Ch%d R] %02X %02X %02X", MAX96717_96716I2cChannel[VinID], SlaveID, Addr, pRxData[0U], 0U);
        RetVal = MAX96717_96716_ERR_NONE;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96717_96716_WaitLockStatus
 *
 *  @Description:: Wait for Max96717_96716 link to be established
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
static UINT32 Max96717_96716_WaitLockStatus(UINT32 VinID, UINT8 SlaveID, UINT16 EnabledLinkID, UINT32 TimeOut)
{
    UINT32 RetVal = MAX96717_96716_ERR_NONE;
    UINT32 Count = 0U;
    UINT8 RxData;

    AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "Max96717_96716_WaitLockStatus: SlaveID 0x%02x", SlaveID, 0U, 0U, 0U, 0U);

    if ((EnabledLinkID & 0x1U) > 0U) {
        RxData = 0x0U;
        while (((RxData & 0x08U) == 0x0U) && (Count < TimeOut)) {

            RetVal |= Max96717_96716_RegRead(VinID, SlaveID, 0x0013U, &RxData);
            if (RetVal != MAX96717_96716_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "[MAX96717_96716] GMSL2 LinkA WaitLockStatus fail", 0U, 0U, 0U, 0U, 0U);
                break;
            }

            (void) AmbaKAL_TaskSleep(1);
            Count++;
        }

        if (Count == TimeOut) {
            AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "[MAX96717_96716] 96716 Reg0x0013=0x%02x (%d) - GMSL2 LinkA NOT locked!", RxData, Count, 0U, 0U, 0U);
            RetVal |= MAX96717_96716_ERR_SERDES_LINK;
        } else {
            AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "[MAX96717_96716] 96716 Reg0x0013=0x%02x (%d) - GMSL2 LinkA Locked!", RxData, Count, 0U, 0U, 0U);
        }
    }
    if ((EnabledLinkID & 0x2U) > 0U) {
        RxData = 0x0U;
        while (((RxData & 0x08U) == 0x0U) && (Count < TimeOut)) {

            RetVal |= Max96717_96716_RegRead(VinID, SlaveID, 0x5009U, &RxData);
            if (RetVal != MAX96717_96716_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "[MAX96717_96716] GMSL2 LinkB WaitLockStatus fail", 0U, 0U, 0U, 0U, 0U);
                break;
            }

            (void) AmbaKAL_TaskSleep(1);
            Count++;
        }

        if (Count == TimeOut) {
            AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "[MAX96717_96716] 96716 Reg0x5009=0x%02x (%d) - GMSL2 LinkB NOT locked!", RxData, Count, 0U, 0U, 0U);
            RetVal |= MAX96717_96716_ERR_SERDES_LINK;
        } else {
            AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "[MAX96717_96716] 96716 Reg0x5009=0x%02x (%d) - GMSL2 LinkB Locked!", RxData, Count, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

UINT32 Max96717_96716_DeInit(UINT32 VinID)
{
    MAX96717_96716_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    pCurSerDesInfo->InitDone = 0;

    return MAX96717_96716_ERR_NONE;
}

UINT32 Max96717_96716_Init(UINT32 VinID, UINT16 EnabledLinkID)
{
    MAX96717_96716_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX96717_96716_ERR_NONE;
    UINT32 i, LinkID;
    UINT8 Data = 0U;
    UINT8 LinkLocked[MAX96716_NUM_RX_PORT] = {0};
    // UINT8 CurDesInitDone = 0U;

    static UINT8 IdcAddrMax96716[MAX96716_MAX_NUM] = {
        IDC_ADDR_MAX96716_0,
        IDC_ADDR_MAX96716_0,
        IDC_ADDR_MAX96716_0,
    };

    static UINT8 IdcAddrMAX96717[MAX96716_MAX_NUM][MAX96716_NUM_RX_PORT] = {
        {IDC_ADDR_MAX96717_0_A, IDC_ADDR_MAX96717_0_B},
        {IDC_ADDR_MAX96717_0_A, IDC_ADDR_MAX96717_0_B},
        {IDC_ADDR_MAX96717_0_A, IDC_ADDR_MAX96717_0_B},
    };

    /* LinkA or LinkB or LinkA & LinkB */
    EnabledLinkID &= 0x3U;
    if (EnabledLinkID == 0U) {
        RetVal = MAX96717_96716_ERR_ARG;
    }

    pCurSerDesInfo->DesAddr = IdcAddrMax96716[VinID];
    pCurSerDesInfo->SerAddr0 = IdcAddrMAX96717[VinID][0];
    pCurSerDesInfo->SerAddr1 = IdcAddrMAX96717[VinID][1];
    pCurSerDesInfo->EnabledLinkID |= (UINT8) EnabledLinkID;

    AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "[MAX96717_96716] Init, Vin(%d), Link(%d) to be built, Total Link(%d) on DES", VinID, EnabledLinkID, pCurSerDesInfo->EnabledLinkID, 0U, 0U);

    if (pCurSerDesInfo->InitDone != 0U) {
        /* disable RCLK iutput */
        if ((EnabledLinkID & 0x1U) > 0U) {
            RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x0006, 0x90);
            // Disable PHY0/1 on deserializer
            Data = 0x00U;
            RetVal |= Max96717_96716_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0332U, &Data);
            Data &= 0xCFU;
            RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0332U, Data);
        }
        if ((EnabledLinkID & 0x2U) > 0U) {
            RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x0006, 0x90);
            // Disable PHY2/3 on deserializer
            Data = 0x00U;
            RetVal |= Max96717_96716_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0332U, &Data);
            Data &= 0x3FU;
            RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0332U, Data);
        }

        if (Max96717_96716_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, EnabledLinkID, 200U) != MAX96717_96716_ERR_NONE) {
            RetVal = MAX96717_96716_ERR_SERDES_LINK;
        }
    }

    /* Update INIT status of other VIN which has the same Deserializer */
    // for (i = 0U; i < MAX96716_MAX_NUM; i++) {
    //     if ((i != VinID) && (IdcAddrMax96716[i] == pCurSerDesInfo->DesAddr) && (MaxSerDesInfo[i].InitDone != 0U)) {
    //         CurDesInitDone = 1;
    //     }
    // }
    // if (CurDesInitDone == 0U) {
    (void)AmbaKAL_TaskSleep(20); //LOCK takes at least 20ms after PWDN

    AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "[MAX96717_96716] De-serializer Initialization", pCurSerDesInfo->DesAddr, 0U, 0U, 0U, 0U);
    /* DES Cfg :3Gbps + COAX */
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0001U, 0x01);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0011U, 0x0F);

    // Disable all PHYs on deserializer
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0332U, 0x04);
    // Disable MAX96716 MIPI CSI
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0313U, 0x00);

    /* Errata RevID=3, item2: I2C access across the link can lock up if timeout occurs */
    /* Set internal I2C slave to I2C Fast-mode speed */
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0040U, 0x15);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x004CU, 0x25);

    /* Errata RevID=3, item5: GMSL Receiver requires configuration register writes for robust operation */
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x143FU, 0x3D);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x143EU, 0xFD);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x14ADU, 0x68);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x14ACU, 0xA8);
    //RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1418U, 0x07);//This write is required only for 6Gbps operation.
    //RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x141FU, 0xC2);//This write is required only for 6Gbps operation.
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x148CU, 0x10);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1498U, 0xC0);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1446U, 0x01);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1445U, 0x81);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x140BU, 0x44);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x140AU, 0x08);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1431U, 0x18);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1421U, 0x08);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x14A5U, 0x70);

    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x153FU, 0x3D);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x153EU, 0xFD);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x15ADU, 0x68);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x15ACU, 0xA8);
    //RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1518U, 0x07);//This write is required only for 6Gbps operation.
    //RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x151FU, 0xC2);//This write is required only for 6Gbps operation.
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x158CU, 0x10);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1598U, 0xC0);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1546U, 0x01);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1545U, 0x81);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x150BU, 0x44);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x150AU, 0x08);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1531U, 0x18);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1521U, 0x08);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x15A5U, 0x70);

    /* Errata RevID=3, item7: GMSL Lock can be lost due to high jitter in a temperature window */
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0302U, 0x10);

    /* One-Shot Link Reset */
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x31);
    RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0012U, 0x24);
    // }

    /* Check current link status */
    for (i = 0U; i < MAX96716_NUM_RX_PORT; i++) {
        LinkID = (UINT32) 0x1U << i;
        if (Max96717_96716_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, (UINT16) LinkID, 200U) != MAX96717_96716_ERR_NONE) {
            LinkLocked[i] = 0U;
        } else {
            LinkLocked[i] = 1U;
        }
    }

    if (pCurSerDesInfo->EnabledLinkID == 0x1U) {
        /* Select single A link configuration */
        RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x01);

        /* Link B is selected */
        if (LinkLocked[1] == 1U) {
            RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0013U, 0x01); //RESET Link B
            (void) AmbaKAL_TaskSleep(100);
            RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0013U, 0x00); //Release RESET Link B
            (void) AmbaKAL_TaskSleep(100);
        }

        if (Max96717_96716_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, EnabledLinkID, 200U) != MAX96717_96716_ERR_NONE) {
            RetVal = MAX96717_96716_ERR_SERDES_LINK;
        } else {
            if ((EnabledLinkID & 1U) != 0U) {
                /* Disable DES remote communication channel link-B over GSML2 */
                if (Max96717_96716_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0003U, &Data) == MAX96717_96716_ERR_NONE) {
                    Data |= 0x04U; /* Bit[2] = 1 */
                    (void) Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0003U, Data);
                }

                /* Remap Device Address */
                (void) Max96717_96716_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr0);;
                (void) AmbaKAL_TaskSleep(100);
                (void) Max96717_96716_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT2, 0x0000, pCurSerDesInfo->SerAddr0);;
                (void) AmbaKAL_TaskSleep(100);
                RetVal |= Max96717_96716_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0000, &Data);
                AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "VinID(%d) Remap Ser SlaveID: 0x%x/0x%x -> 0x%x", VinID, IDC_ADDR_MAX96717_DEFAULT, IDC_ADDR_MAX96717_DEFAULT2, Data, 0U);

                /* Enable DES remote communication channel link-B over GSML2 */
                if (Max96717_96716_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0003U, &Data) == MAX96717_96716_ERR_NONE) {
                    Data &= 0xFBU; /* Bit[2] = 0 */
                    (void) Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0003U, Data);
                }
            }
        }
    }

    if (pCurSerDesInfo->EnabledLinkID == 0x2U) {
        /* Select single B link configuration*/
        RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x02);

        /* Link A is selected */
        if (LinkLocked[0] == 1U) {
            RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x42); //RESET Link A (0x40 | 0x02)
            (void) AmbaKAL_TaskSleep(100);
            RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x02); //Release RESET Link A (0x00 | 0x02)
            (void) AmbaKAL_TaskSleep(100);
        }

        if (Max96717_96716_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, EnabledLinkID, 200U) != MAX96717_96716_ERR_NONE) {
            RetVal = MAX96717_96716_ERR_SERDES_LINK;
        } else {
            if ((EnabledLinkID & 2U) != 0U) {
                /* Disable DES remote communication channel link-A over GSML2 */
                if (Max96717_96716_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0001U, &Data) == MAX96717_96716_ERR_NONE) {
                    Data |= 0x10U; /* Bit[4] = 1 */
                    (void) Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0001U, Data);
                }

                /* Remap Device Address */
                (void) Max96717_96716_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr1);
                (void) Max96717_96716_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT2, 0x0000, pCurSerDesInfo->SerAddr1);
                RetVal |= Max96717_96716_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0000, &Data);
                AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "VinID(%d) Remap Ser SlaveID: 0x%x/0x%x -> 0x%x", VinID, IDC_ADDR_MAX96717_DEFAULT, IDC_ADDR_MAX96717_DEFAULT2, Data, 0U);

                /* Enable DES remote communication channel link-A over GSML2 */
                if (Max96717_96716_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0001U, &Data) == MAX96717_96716_ERR_NONE) {
                    Data &= 0xEFU; /* Bit[4] = 0 */
                    (void) Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0001U, Data);
                }
            }
        }
    }

    if (pCurSerDesInfo->EnabledLinkID == 0x3U) {
        /* Link A & B is selected */
        RetVal |= Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x03);

        if (Max96717_96716_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, EnabledLinkID, 200U) != MAX96717_96716_ERR_NONE) {
            RetVal = MAX96717_96716_ERR_SERDES_LINK;
        } else {
            if ((EnabledLinkID & 1U) != 0U) {
                /* Disable DES remote communication channel link-B over GSML2 */
                if (Max96717_96716_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0003U, &Data) == MAX96717_96716_ERR_NONE) {
                    Data |= 0x04U; /* Bit[2] = 1 */
                    (void) Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0003U, Data);
                }

                /* Remap Device Address */
                (void) Max96717_96716_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr0);
                (void) Max96717_96716_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT2, 0x0000, pCurSerDesInfo->SerAddr0);
                RetVal |= Max96717_96716_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0000, &Data);
                AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "VinID(%d) Remap Ser SlaveID: 0x%x/0x%x -> 0x%x", VinID, IDC_ADDR_MAX96717_DEFAULT, IDC_ADDR_MAX96717_DEFAULT2, Data, 0U);

                /* Enable DES remote communication channel link-B over GSML2 */
                if (Max96717_96716_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0003U, &Data) == MAX96717_96716_ERR_NONE) {
                    Data &= 0xFBU; /* Bit[2] = 0 */
                    (void) Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0003U, Data);
                }
            }

            if ((EnabledLinkID & 2U) != 0U) {
                /* Disable DES remote communication channel link-A over GSML2 */
                if (Max96717_96716_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0001U, &Data) == MAX96717_96716_ERR_NONE) {
                    Data |= 0x10U; /* Bit[4] = 1 */
                    (void) Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0001U, Data);
                }

                /* Remap Device Address */
                (void) Max96717_96716_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr1);
                (void) Max96717_96716_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT2, 0x0000, pCurSerDesInfo->SerAddr1);
                RetVal |= Max96717_96716_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0000, &Data);
                AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "VinID(%d) Remap Ser SlaveID: 0x%x/0x%x -> 0x%x", VinID, IDC_ADDR_MAX96717_DEFAULT, IDC_ADDR_MAX96717_DEFAULT2, Data, 0U);

                /* Enable DES remote communication channel link-A over GSML2 */
                if (Max96717_96716_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0001U, &Data) == MAX96717_96716_ERR_NONE) {
                    Data &= 0xEFU; /* Bit[4] = 0 */
                    (void) Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0001U, Data);
                }
            }
        }
    }

    /* Update link status of other VIN which has the same Deserializer */
    for (i = 0; i < MAX96716_MAX_NUM; i++) {
        if ((i != VinID) && (IdcAddrMax96716[i] == pCurSerDesInfo->DesAddr)) {
            MaxSerDesInfo[i].EnabledLinkID |= pCurSerDesInfo->EnabledLinkID;
        }
    }
    pCurSerDesInfo->InitDone = 1U;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96716_Config
 *
 *  @Description:: Configure Max96717_96716
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
UINT32 Max96717_96716_Config(UINT32 VinID, const MAX96717_96716_SERDES_CONFIG_s *pSerdesConfig)
{
    const MAX96717_96716_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX96717_96716_ERR_NONE;
    UINT16 EnabledLinkID;
    UINT8 SensorSlaveID, SensorAliasID;
    UINT8 SerAddr, DesAddr;
    UINT8 Data;

    if (pSerdesConfig == NULL) {
        RetVal = MAX96717_96716_ERR_ARG;
    } else {
        EnabledLinkID   = pSerdesConfig->EnabledLinkID;
        SensorSlaveID   = pSerdesConfig->SensorSlaveID;
        SensorAliasID   = 0U;
        SerAddr         = 0U;
        DesAddr         = pCurSerDesInfo->DesAddr;

        if ((pCurSerDesInfo->EnabledLinkID & EnabledLinkID) != EnabledLinkID) {
            AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "[MAX96717_96716] Config NG (EnabledLinkID: 0x%02x is not built)", EnabledLinkID, 0U, 0U, 0U, 0U);
            RetVal = MAX96717_96716_ERR_SERDES_LINK;
        } else {
            AmbaPrint_ModulePrintUInt5(MAX96717_96716_MODULE_ID, "[MAX96717_96716] Config (EnabledLinkID: 0x%02x)...", EnabledLinkID, 0U, 0U, 0U, 0U);
        }

        if ((EnabledLinkID & 0x1U) != 0U) {
            SerAddr = pCurSerDesInfo->SerAddr0;
            SensorAliasID = pSerdesConfig->SensorAliasID[0];

            /* SER96717: MIPI Port B -> Pipe Z (TX Stream ID=2) */
            Data = 0x20U | (pSerdesConfig->NumDataBits & 0x1fU);
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x031e, Data); /* Software override enable for Video Pipe Z */
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0308, 0x64); /* Select CSI port B for Video Pipe Z */
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0311, 0x40); /* Start Video Pipe Z from CSI port B */
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x005B, 0x02); /* Set Video Pipe Z Stream ID = 2 */

            /* DES96716: Link A -> Pipe Y -> Controller 1 (Link A Stream ID = 2 for Video Pipe Y, Link B Stream ID = 2 for Video Pipe Z) */
            RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0161, 0x32);

            if (pSerdesConfig->EnableExtFrameSync > 0U) {
                // -----------------------------------------------------------
                // GPIO forwarding setup
                // -----------------------------------------------------------
                /* DES_MFPx(GPIOx) as input and source enabled for GMSL2 transmission */
                Data = 0x40U | pSerdesConfig->FrameSyncSel.DesGpio;
                RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, (0x2B0U + ((UINT16) pSerdesConfig->FrameSyncSel.DesGpio * 3U)), 0x63);
                RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, (0x2B1U + ((UINT16) pSerdesConfig->FrameSyncSel.DesGpio * 3U)), Data);/* TX ID */
                RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, (0x2B2U + ((UINT16) pSerdesConfig->FrameSyncSel.DesGpio * 3U)), 0x40);

                /* SER_MFPx(GPIOx) as output and source enabled for GMSL2 reception */
                Data = 0x40U | pSerdesConfig->FrameSyncSel.DesGpio;
                RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, (0x2BEU + ((UINT16) pSerdesConfig->FrameSyncSel.SerGpio * 3U)), 0x84);
                RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, (0x2BFU + ((UINT16) pSerdesConfig->FrameSyncSel.SerGpio * 3U)), 0xb0);
                RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, (0x2C0U + ((UINT16) pSerdesConfig->FrameSyncSel.SerGpio * 3U)), Data);/* RX ID */
            }

            // -----------------------------------------------------------
            // Serializer MIPI Setup
            // -----------------------------------------------------------
            /* Set MIPI_RX registers number of lanes 1x4 mode */
            (void) Max96717_96716_RegWrite(VinID, SerAddr, 0x0330, 0x00);
            /* Select number of data lanes for Port B. */
            Data = (UINT8)((pSerdesConfig->CSIRxLaneNum - 1U) << 4U);
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0331, Data);
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0332, 0xEE);
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0333, 0xE4);

            // -----------------------------------------------------------
            //  sensor I2C tanslation
            // -----------------------------------------------------------
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);

            // -----------------------------------------------------------
            //  Customized: MFP4 & MPF7 faster slew rate
            // -----------------------------------------------------------
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0570, 0x1C);
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0571, 0xEC);
        }
        if ((EnabledLinkID & 0x2U) != 0U) {
            SerAddr = pCurSerDesInfo->SerAddr1;
            SensorAliasID = pSerdesConfig->SensorAliasID[1];

            /* SER96717: MIPI Port B -> Pipe Z (TX Stream ID=2) */
            Data = 0x20U | (pSerdesConfig->NumDataBits & 0x1fU);
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x031e, Data); /* Software override enable for Video Pipe Z */
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0308, 0x64); /* Select CSI port B for Video Pipe Z */
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0311, 0x40); /* Start Video Pipe Z from CSI port B */
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x005B, 0x02); /* Set Video Pipe Z Stream ID = 2*/

            /* DES96716: Link B -> Pipe Z -> Controller 2 (Link A Stream ID = 2 for Video Pipe Y, Link B Stream ID = 2 for Video Pipe Z) */
            RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0161, 0x32);

            if (pSerdesConfig->EnableExtFrameSync> 0U) {
                // -----------------------------------------------------------
                // GPIO forwarding setup
                // -----------------------------------------------------------
                /* DES_MFPx(GPIOx) as input and source enabled for GMSL2 transmission */
                Data = 0x40U | pSerdesConfig->FrameSyncSel.DesGpio;
                RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, (0x52B0U + ((UINT16) pSerdesConfig->FrameSyncSel.DesGpio * 3U)), 0x63);
                RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, (0x52B1U + ((UINT16) pSerdesConfig->FrameSyncSel.DesGpio * 3U)), Data);
                RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, (0x52B2U + ((UINT16) pSerdesConfig->FrameSyncSel.DesGpio * 3U)), 0x40);

                /* SER_MFPx(GPIOx) as output and source enabled for GMSL2 reception */
                Data = 0x40U | pSerdesConfig->FrameSyncSel.DesGpio;
                RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, (0x2BEU + ((UINT16) pSerdesConfig->FrameSyncSel.SerGpio * 3U)), 0x84);
                RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, (0x2BFU + ((UINT16) pSerdesConfig->FrameSyncSel.SerGpio * 3U)), 0xb0);
                RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, (0x2C0U + ((UINT16) pSerdesConfig->FrameSyncSel.SerGpio * 3U)), Data);
            }

            // -----------------------------------------------------------
            // Serializer MIPI Setup
            // -----------------------------------------------------------
            /* Set MIPI_RX registers number of lanes 1x4 mode */
            (void) Max96717_96716_RegWrite(VinID, SerAddr, 0x0330, 0x00);
            /* Select number of data lanes for Port B. */
            Data = (UINT8)((pSerdesConfig->CSIRxLaneNum - 1U) << 4U);
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0331, Data);
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0332, 0xEE);
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0333, 0xE4);

            // -----------------------------------------------------------
            //  sensor I2C tanslation
            // -----------------------------------------------------------
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);

            // -----------------------------------------------------------
            //  Customized: MFP4 & MPF7 faster slew rate
            // -----------------------------------------------------------
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0570, 0x1C);
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0571, 0xEC);
        }

        // -----------------------------------------------------------
        // Deserializer MIPI Setup
        // -----------------------------------------------------------
        // Output 2x4 mode (portA:PHY0/1, portB:PHY2/3)
        RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0330, 0x04);

        // CSI lane mapping
        RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0333, 0x4E);
        RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0334, 0xE4);

        if (EnabledLinkID == 0x1U) {
            // Enable PHY0/1 on deserializer
            Data = 0x00;
            RetVal |= Max96717_96716_RegRead(VinID, DesAddr, 0x0332, &Data);
            Data |= 0x30U;
            RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0332, Data);

            // lane count on controller 1 (port A)
            Data = 0x10U | (UINT8)((pSerdesConfig->CSITxLaneNum[0] - 1U) << 6U);
            RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x044A, Data);

            // Deserializer port clock rates (controller 1 for Port A, controller 2 for Port B in 2x4 mode)
            Data = 0x20U | pSerdesConfig->CSITxSpeed[0]; /* CSITxSpeed * 100 Mbps */
            RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0320, Data);

            /* Enable FEC in deserializer first. When enabled deserializer sends 1 info frame to serializer. */
            Data = 0x62U;
            RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0028, Data);

            /* Enable FEC in serializer next. When enabled serializer sends 1 info frame to deserializer. deserializer must expect this frame. */
            Data = 0x62U;
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0028, Data);

            /* Clear all FEC stats and counters */
            Data = 0x01U;
            RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x2000, Data);
        }

        if (EnabledLinkID == 0x2U) {
            // Enable PHY2/3 on deserializer
            Data = 0x00;
            RetVal |= Max96717_96716_RegRead(VinID, DesAddr, 0x0332, &Data);
            Data |= 0xC0U;
            RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0332, Data);

            // lane count on controller 2 (port B)
            Data = 0x10U | (UINT8)((pSerdesConfig->CSITxLaneNum[1] - 1U) << 6U);
            RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x048A, Data);

            // Deserializer port clock rates (controller 1 for Port A, controller 2 for Port B in 2x4 mode)
            Data = 0x20U | pSerdesConfig->CSITxSpeed[1]; /* CSITxSpeed * 100 Mbps */
            RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0323, Data);

            /* Enable FEC in deserializer first. When enabled deserializer sends 1 info frame to serializer. */
            Data = 0x62U;
            RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x5028, Data);

            /* Enable FEC in serializer next. When enabled serializer sends 1 info frame to deserializer. deserializer must expect this frame. */
            Data = 0x62U;
            RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0028, Data);

            /* Clear all FEC stats and counters */
            Data = 0x01U;
            RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x2100, Data);
        }

        // Enable MAX96716 MIPI CSI
        Data = 0x02U;
        RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0313, Data);

        // -----------------------------------------------------------
        // Enable TUNNELING mode
        // -----------------------------------------------------------
        Data = 0x80U;
        RetVal |= Max96717_96716_RegWrite(VinID, SerAddr, 0x0383, Data);
        Data = 0x09U;
        RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0474, Data);
        Data = 0x0fU;
        RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x04B4, Data);

    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96716_SetSensorClk
 *
 *  @Description:: Set MAX96717 reference clock output for image sensor
 *
 *  @Input      ::
 *      VinID:     VIN ID
 *      ChipID:    Chip ID
 *      RClkOut:   Reference clock output to MFP2
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96717_96716_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkOut)
{
    UINT32 RetVal = MAX96717_96716_ERR_NONE;
    UINT8 SlaveID = 0x0U;
    const MAX96717_96716_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX96717_96716_ERR_INVALID_API;
    } else {
        if (ChipID == MAX96717_A_ID) {
            SlaveID = pCurSerDesInfo->SerAddr0;
        } else if (ChipID == MAX96717_B_ID) {
            SlaveID = pCurSerDesInfo->SerAddr1;
        } else {
            RetVal = MAX96717_96716_ERR_INVALID_API;
        }

        if (RetVal == MAX96717_96716_ERR_NONE) {
            if (RClkOut == MAX96717_96716_CLKFREQ_25M_MFP2) {
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x0003, 0x04); /* MFP2 to output RCLKOUT XTAL/1=25MHz */
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x0006, 0xb0); /* enable RCLK output */

            } else if (RClkOut == MAX96717_96716_CLKFREQ_24M_MFP2) {
                /* pre-defined clock setting is disabled */
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x03F0, 0x02);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x03F4, 0x0A);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x03F5, 0x07);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x03F0, 0x00);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x1A03, 0x10);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x1A07, 0x04);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x1A08, 0x3D);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x1A09, 0x40);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x1A0A, 0x40);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x1A0B, 0x60);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x03F0, 0x01);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x0003, 0x07);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x0006, 0xb0);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x0570, 0x00);

            } else if (RClkOut == MAX96717_96716_CLKFREQ_27M_MFP2) {
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x03f0, 0x02); /* reset reference generation PLL */
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x03f0, 0x51); /* enable pre-defined clock 27Mhz */
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x0003, 0x07); /* MFP2 to output RCLKOUT */
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x056f, 0x0e); /* MFP2 rise/fall time speed setting */
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x0006, 0xb0); /* enable RCLK output */

            } else if (RClkOut == MAX96717_96716_CLKFREQ_25M_MFP4) {
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x0003, 0x00); /* MFP4 to output RCLKOUT XTAL/1=25MHz */
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x0006, 0xb0); /* enable RCLK output */

            } else if (RClkOut == MAX96717_96716_CLKFREQ_24M_MFP4) {
                /* pre-defined clock setting is disabled */
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x03F0, 0x02);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x03F4, 0x0A);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x03F5, 0x07);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x03F0, 0x00);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x1A03, 0x10);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x1A07, 0x04);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x1A08, 0x3D);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x1A09, 0x40);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x1A0A, 0x40);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x1A0B, 0x60);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x03F0, 0x01);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x0003, 0x03);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x0006, 0xb0);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, 0x0570, 0x00);
            } else {
                RetVal = MAX96717_96716_ERR_ARG;
            }
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96716_SetGpioOutput
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
UINT32 Max96717_96716_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT8 Level)
{
    UINT32 RetVal = MAX96717_96716_ERR_NONE;
    UINT16 Addr = 0U; /* register address */
    MAX96717_96716_GPIO_A_REG_s GpioAReg = {0};
    MAX96717_96716_GPIO_B_REG_s GpioBReg = {0};
    const MAX96717_96716_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT8 SlaveID = 0U, RegAVal, RegBVal;

    if (ChipID == MAX96716_ID) {
        Addr = (UINT16)0x2b0 + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->DesAddr;
    } else if (ChipID == MAX96717_A_ID) {
        Addr = (UINT16)0x2be + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->SerAddr0;
    } else if (ChipID == MAX96717_B_ID) {
        Addr = (UINT16)0x2be + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->SerAddr1;
    } else {
        RetVal = MAX96717_96716_ERR_ARG;
    }
    if (RetVal == MAX96717_96716_ERR_NONE) {

        if (PinID >= MAX96717_96716_NUM_GPIO_PIN) {
            RetVal = MAX96717_96716_ERR_ARG;
        } else {
            RetVal |= Max96717_96716_RegRead(VinID, SlaveID, Addr, &RegAVal);
            RetVal |= Max96717_96716_RegRead(VinID, SlaveID, Addr + 1U, &RegBVal);
            if (RetVal == MAX96717_96716_ERR_NONE) {
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
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, Addr, RegAVal);
                RetVal |= Max96717_96716_RegWrite(VinID, SlaveID, Addr + 1U, RegBVal);
            } else {
                RetVal = MAX96717_96716_ERR_ARG;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96716_RegisterWrite
 *
 *  @Description:: Write Max96717_96716 registers API
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
UINT32 Max96717_96716_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 Data)
{
    UINT32 RetVal = MAX96717_96716_ERR_NONE;
    const MAX96717_96716_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX96717_96716_ERR_INVALID_API;
    } else {
        if (ChipID == MAX96716_ID) {
            RetVal = Max96717_96716_RegWrite(VinID, pCurSerDesInfo->DesAddr, Addr, Data);
        } else if (ChipID == MAX96717_A_ID) {
            RetVal = Max96717_96716_RegWrite(VinID, pCurSerDesInfo->SerAddr0, Addr, Data);
        } else if (ChipID == MAX96717_B_ID) {
            RetVal = Max96717_96716_RegWrite(VinID, pCurSerDesInfo->SerAddr1, Addr, Data);
        } else {
            RetVal = MAX96717_96716_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96716_RegisterRead
 *
 *  @Description:: Read Max96717_96716 registers API
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
UINT32 Max96717_96716_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT8 Addr, UINT8 *pData)
{
    UINT32 RetVal = MAX96717_96716_ERR_NONE;
    const MAX96717_96716_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX96717_96716_ERR_INVALID_API;
    } else {
        if (ChipID == MAX96716_ID) {
            RetVal = Max96717_96716_RegRead(VinID, pCurSerDesInfo->DesAddr, Addr, pData);
        } else if (ChipID == MAX96717_A_ID) {
            RetVal = Max96717_96716_RegRead(VinID, pCurSerDesInfo->SerAddr0, Addr, pData);
        } else if (ChipID == MAX96717_B_ID) {
            RetVal = Max96717_96716_RegRead(VinID, pCurSerDesInfo->SerAddr1, Addr, pData);
        } else {
            RetVal = MAX96717_96716_ERR_ARG;
        }
    }

    return RetVal;
}
