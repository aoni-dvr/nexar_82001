/**
 *  @file AmbaSbrg_Max96717_96722.c
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
 *  @details MAX96717 & MAX96722 driver APIs
 *
 */

#include <AmbaWrap.h>
#include "AmbaKAL.h"
#include "AmbaI2C.h"
#include "AmbaPrint.h"
#include "AmbaVIN.h"
#include "AmbaSbrg_Max96717_96722.h"

#include "bsp.h"

#define MAX96722_MAX_NUM        AMBA_NUM_VIN_CHANNEL

typedef struct {
    UINT32 InitDone;
    UINT8  DesAddr;          /* Max96722 I2C Slave Addr */
    UINT8  SerAddr0;         /* Distinguishable MAX96717 LinkA I2C Slave Addr */
    UINT8  SerAddr1;         /* Distinguishable MAX96717 LinkB I2C Slave Addr */
    UINT8  EnabledLinkID;
} MAX96717_96722_SERDES_INFO_s;

static UINT32 MAX96717_96722I2cChannel[MAX96722_MAX_NUM] = {
    [AMBA_VIN_CHANNEL0] = AMBA_MAXIM_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_MAXIM_I2C_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_MAXIM_I2C_CHANNEL_PIP2,
};

static MAX96717_96722_SERDES_INFO_s MaxSerDesInfo[MAX96722_MAX_NUM] = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96717_96722_RegWrite
 *
 *  @Description:: Write Max96717_96722 registers
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
static UINT32 Max96717_96722_RegWrite(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 Data)
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

    if (AmbaI2C_MasterWrite(MAX96717_96722I2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                            &I2cConfig, &TxSize, 1000U) != OK) {
        AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "SlaveID 0x%02x Addr 0x%04x I2C does not work!!!!!", SlaveID, Addr, 0U, 0U, 0U);
        RetVal = MAX96717_96722_ERR_COMMUNICATE;
    } else {
        AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "[MAX96717_96722 Ch%d W] %02X %02X %02X", MAX96717_96722I2cChannel[VinID], SlaveID, Addr, Data, 0U);
        RetVal = MAX96717_96722_ERR_NONE;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96717_96722_RegRead
 *
 *  @Description:: Read Max96717_96722 registers
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
static UINT32 Max96717_96722_RegRead(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 *pRxData)
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

    if (AmbaI2C_MasterReadAfterWrite(MAX96717_96722I2cChannel[VinID], AMBA_I2C_SPEED_FAST,
                                     1U, &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U) != OK) {
        AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "SlaveID 0x%02x Addr 0x%04x I2C does not work!!!!!", SlaveID, Addr, 0U, 0U, 0U);
        RetVal = MAX96717_96722_ERR_COMMUNICATE;
    } else {
        // AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "[MAX96717_96722 Ch%d R] %02X %02X %02X", MAX96717_96722I2cChannel[VinID], SlaveID, Addr, pRxData[0U], 0U);
        RetVal = MAX96717_96722_ERR_NONE;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96717_96722_WaitLockStatus
 *
 *  @Description:: Wait for Max96717_96722 link to be established
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
static UINT32 Max96717_96722_WaitLockStatus(UINT32 VinID, UINT8 SlaveID, UINT16 EnabledLinkID, UINT32 TimeOut)
{
    UINT32 RetVal = MAX96717_96722_ERR_NONE;
    UINT32 Count = 0U;
    UINT8 RxData;

    AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "Max96717_96722_WaitLockStatus: SlaveID 0x%02x, EnabledLinkID=0x%x", SlaveID, EnabledLinkID, 0U, 0U, 0U);

    if ((EnabledLinkID & 0x1U) > 0U) {
        RxData = 0x0U;
        while (((RxData & 0x08U) == 0x0U) && (Count < TimeOut)) {

            RetVal |= Max96717_96722_RegRead(VinID, SlaveID, 0x001aU, &RxData);
            if (RetVal != MAX96717_96722_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "[MAX96717_96722] GMSL2 LinkA WaitLockStatus fail", 0U, 0U, 0U, 0U, 0U);
                break;
            }

            (void) AmbaKAL_TaskSleep(1);
            Count++;
        }

        if (Count == TimeOut) {
            AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "[MAX96717_96722] 96722 Reg0x001a=0x%02x (%d) - GMSL2 LinkA NOT locked!", RxData, Count, 0U, 0U, 0U);
            RetVal |= MAX96717_96722_ERR_SERDES_LINK;
        } else {
            AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "[MAX96717_96722] 96722 Reg0x001a=0x%02x (%d) - GMSL2 LinkA Locked!", RxData, Count, 0U, 0U, 0U);
        }
    }

    if ((EnabledLinkID & 0x2U) > 0U) {
        RxData = 0x0U;
        while (((RxData & 0x08U) == 0x0U) && (Count < TimeOut)) {

            RetVal |= Max96717_96722_RegRead(VinID, SlaveID, 0x000aU, &RxData);
            if (RetVal != MAX96717_96722_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "[MAX96717_96722] GMSL2 LinkB WaitLockStatus fail", 0U, 0U, 0U, 0U, 0U);
                break;
            }

            (void) AmbaKAL_TaskSleep(1);
            Count++;
        }

        if (Count == TimeOut) {
            AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "[MAX96717_96722] 96722 Reg0x000a=0x%02x (%d) - GMSL2 LinkB NOT locked!", RxData, Count, 0U, 0U, 0U);
            RetVal |= MAX96717_96722_ERR_SERDES_LINK;
        } else {
            AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "[MAX96717_96722] 96722 Reg0x000a=0x%02x (%d) - GMSL2 LinkB Locked!", RxData, Count, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

UINT32 Max96717_96722_DeInit(UINT32 VinID)
{
    MAX96717_96722_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    pCurSerDesInfo->InitDone = 0;

    return MAX96717_96722_ERR_NONE;
}

UINT32 Max96717_96722_Init(UINT32 VinID, UINT16 EnabledLinkID)
{
    MAX96717_96722_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX96717_96722_ERR_NONE;
    UINT32 i;
    UINT8 Data = 0U;
    // UINT8 LinkLocked[MAX96722_NUM_RX_PORT] = {0};
    // UINT8 CurDesInitDone = 0U;

    static UINT8 IdcAddrMax96722[MAX96722_MAX_NUM] = {
        IDC_ADDR_MAX96722_0,
        IDC_ADDR_MAX96722_0,
        IDC_ADDR_MAX96722_0,
    };

    static UINT8 IdcAddrMAX96717[MAX96722_MAX_NUM][MAX96722_NUM_RX_PORT] = {
        {IDC_ADDR_MAX96717_0_A, IDC_ADDR_MAX96717_0_B},
        {IDC_ADDR_MAX96717_0_A, IDC_ADDR_MAX96717_0_B},
        {IDC_ADDR_MAX96717_0_A, IDC_ADDR_MAX96717_0_B},
    };

    /* LinkA or LinkB or LinkA & LinkB */
    EnabledLinkID &= 0x3U;
    if (EnabledLinkID == 0U) {
        RetVal = MAX96717_96722_ERR_ARG;
    }

    pCurSerDesInfo->DesAddr = IdcAddrMax96722[VinID];
    pCurSerDesInfo->SerAddr0 = IdcAddrMAX96717[VinID][0];
    pCurSerDesInfo->SerAddr1 = IdcAddrMAX96717[VinID][1];
    pCurSerDesInfo->EnabledLinkID |= (UINT8) EnabledLinkID;

    AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "[MAX96717_96722] Init, Vin(%d), Link(%d) to be built, Total Link(%d) on DES", VinID, EnabledLinkID, pCurSerDesInfo->EnabledLinkID, 0U, 0U);

    /* Update INIT status of other VIN which has the same Deserializer */
    // for (i = 0U; i < MAX96722_MAX_NUM; i++) {
    //     if ((i != VinID) && (IdcAddrMax96722[i] == pCurSerDesInfo->DesAddr) && (MaxSerDesInfo[i].InitDone != 0U)) {
    //         CurDesInitDone = 1;
    //     }
    // }
    // if (CurDesInitDone == 0U) {
    (void)AmbaKAL_TaskSleep(20); //LOCK takes at least 20ms after PWDN

    AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "[MAX96717_96722] De-serializer Initialization", pCurSerDesInfo->DesAddr, 0U, 0U, 0U, 0U);
    /* DES Cfg :3Gbps + COAX */
    RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0006U, 0xF3);
    // RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x11);//>>>


    // Disable all PHYs on deserializer
    // RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0332U, 0x04);
    // RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x08A2U, 0x04);//>>>
    // Disable MAX96722 MIPI CSI
    // RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0313U, 0x00);
    RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x040BU, 0x00);

    if (pCurSerDesInfo->EnabledLinkID == 0x1U) {
        /* Select single A link configuration */
        RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0006U, 0xF1);
        RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0018U, 0x0F);

        if (Max96717_96722_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, EnabledLinkID, 200U) != MAX96717_96722_ERR_NONE) {
            RetVal = MAX96717_96722_ERR_SERDES_LINK;
        } else {
            if ((EnabledLinkID & 1U) != 0U) {
                /* Remap Device Address */
                (void) Max96717_96722_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr0);;
                (void) AmbaKAL_TaskSleep(100);
                (void) Max96717_96722_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT2, 0x0000, pCurSerDesInfo->SerAddr0);;
                (void) AmbaKAL_TaskSleep(100);
                RetVal |= Max96717_96722_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0000, &Data);
                AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "VinID(%d) Remap Ser SlaveID: 0x%x/0x%x -> 0x%x", VinID, IDC_ADDR_MAX96717_DEFAULT, IDC_ADDR_MAX96717_DEFAULT2, Data, 0U);
            }
        }
    }

    if (pCurSerDesInfo->EnabledLinkID == 0x2U) {
        /* Select single B link configuration*/
        RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0006U, 0xF2);
        RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0018U, 0x0F);

        if (Max96717_96722_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, EnabledLinkID, 200U) != MAX96717_96722_ERR_NONE) {
            RetVal = MAX96717_96722_ERR_SERDES_LINK;
        } else {
            if ((EnabledLinkID & 2U) != 0U) {
                /* Disable DES remote communication channel link-A over GSML2 */

                /* Remap Device Address */
                (void) Max96717_96722_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr1);
                (void) Max96717_96722_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT2, 0x0000, pCurSerDesInfo->SerAddr1);
                RetVal |= Max96717_96722_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0000, &Data);
                AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "VinID(%d) Remap Ser SlaveID: 0x%x/0x%x -> 0x%x", VinID, IDC_ADDR_MAX96717_DEFAULT, IDC_ADDR_MAX96717_DEFAULT2, Data, 0U);

                /* Enable DES remote communication channel link-A over GSML2 */
            }
        }
    }

    if (pCurSerDesInfo->EnabledLinkID == 0x3U) {
        /* Link A & B is selected */
        RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0006U, 0xF3);
        RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0018U, 0x0F);

        if (Max96717_96722_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, EnabledLinkID, 200U) != MAX96717_96722_ERR_NONE) {
            RetVal = MAX96717_96722_ERR_SERDES_LINK;
        } else {
            if ((EnabledLinkID & 1U) != 0U) {
                /* Disable DES remote communication channel link-B over GSML2 */
                // RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x000E, 0x0E);
                RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0006U, 0xF1);

                /* Remap Device Address */
                (void) Max96717_96722_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr0);
                (void) AmbaKAL_TaskSleep(100);
                (void) Max96717_96722_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT2, 0x0000, pCurSerDesInfo->SerAddr0);
                (void) AmbaKAL_TaskSleep(100);
                RetVal |= Max96717_96722_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0000, &Data);
                AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "VinID(%d) Remap Ser SlaveID: 0x%x/0x%x -> 0x%x", VinID, IDC_ADDR_MAX96717_DEFAULT, IDC_ADDR_MAX96717_DEFAULT2, Data, 0U);

                /* Enable DES remote communication channel link-B over GSML2 */
                RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x000E, 0x0C);
            }

            if ((EnabledLinkID & 2U) != 0U) {
                /* Disable DES remote communication channel link-A over GSML2 */
                // RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x000E, 0x0D);
                RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0006U, 0xF2);

                /* Remap Device Address */
                (void) Max96717_96722_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr1);
                (void) AmbaKAL_TaskSleep(100);
                (void) Max96717_96722_RegWrite(VinID, IDC_ADDR_MAX96717_DEFAULT2, 0x0000, pCurSerDesInfo->SerAddr1);
                (void) AmbaKAL_TaskSleep(100);
                RetVal |= Max96717_96722_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0000, &Data);
                AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "VinID(%d) Remap Ser SlaveID: 0x%x/0x%x -> 0x%x", VinID, IDC_ADDR_MAX96717_DEFAULT, IDC_ADDR_MAX96717_DEFAULT2, Data, 0U);

                /* Enable DES remote communication channel link-A over GSML2 */
                RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x000E, 0x0C);
            }
            RetVal |= Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0006U, 0xF3);
        }
    }
    (void) AmbaKAL_TaskSleep(100);

    /* Update link status of other VIN which has the same Deserializer */
    for (i = 0; i < MAX96722_MAX_NUM; i++) {
        if ((i != VinID) && (IdcAddrMax96722[i] == pCurSerDesInfo->DesAddr)) {
            MaxSerDesInfo[i].EnabledLinkID |= pCurSerDesInfo->EnabledLinkID;
        }
    }
    pCurSerDesInfo->InitDone = 1U;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96722_Config
 *
 *  @Description:: Configure Max96717_96722
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
UINT32 Max96717_96722_Config(UINT32 VinID, const MAX96717_96722_SERDES_CONFIG_s *pSerdesConfig)
{
    const MAX96717_96722_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX96717_96722_ERR_NONE;
    UINT16 EnabledLinkID;
    UINT8 SensorSlaveID, SensorAliasID;
    UINT8 SerAddr, DesAddr;
    UINT8 Data;
    AmbaPrint_PrintUInt5( ">>>>>[Max96717_96722_Config] Config (VinID: 0x%02x )", VinID, 0U, 0U, 0U, 0U);
    if (pSerdesConfig == NULL) {
        RetVal = MAX96717_96722_ERR_ARG;
    } else {
        EnabledLinkID   = pSerdesConfig->EnabledLinkID;
        SensorSlaveID   = pSerdesConfig->SensorSlaveID;
        SensorAliasID   = 0U;
        SerAddr         = 0U;
        DesAddr         = pCurSerDesInfo->DesAddr;

        if ((pCurSerDesInfo->EnabledLinkID & EnabledLinkID) != EnabledLinkID) {
            AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "[MAX96717_96722] Config NG (EnabledLinkID: 0x%02x is not built)", EnabledLinkID, 0U, 0U, 0U, 0U);
            RetVal = MAX96717_96722_ERR_SERDES_LINK;
        } else {
            AmbaPrint_ModulePrintUInt5(MAX96717_96722_MODULE_ID, "[MAX96717_96722] Config (EnabledLinkID: 0x%02x)...", EnabledLinkID, 0U, 0U, 0U, 0U);
        }

        if ((EnabledLinkID & 0x1U) != 0U) {
            SerAddr = pCurSerDesInfo->SerAddr0;
            SensorAliasID = pSerdesConfig->SensorAliasID[0];

            /* SER96717: MIPI Port B -> Pipe Z (TX Stream ID=2) */
            Data = 0x20U | (pSerdesConfig->NumDataBits & 0x1fU);
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x031e, Data); /* Software override enable for Video Pipe Z */
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0308, 0x64); /* Select CSI port B for Video Pipe Z */
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0311, 0x40); /* Start Video Pipe Z from CSI port B */
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x005B, 0x02); /* Set Video Pipe Z Stream ID = 2 */

            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0383, 0x00);
            Data = 0x40U | pSerdesConfig->DataType;
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0318, Data); /*  RAW to pipe Z */
            /* DES96722: Link A -> Pipe Y -> Controller 1 (Link A Stream ID = 2 for Video Pipe Y, Link B Stream ID = 2 for Video Pipe Z) */
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0161, 0x32); //>>>>

            if (pSerdesConfig->EnableExtFrameSync > 0U) {
                // -----------------------------------------------------------
                // GPIO forwarding setup
                // -----------------------------------------------------------
                /* DES_MFPx(GPIOx) as input and source enabled for GMSL2 transmission */
                Data = 0x40U | pSerdesConfig->FrameSyncSel.DesGpio;
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x04A0U, 0x08U); //FSYNC_MODE = Frame sync generation is off, GPIO is used for FSYNC, type = GMSL2
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x04AFU, 0x9FU);
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0306U, 0x83U); //GPIO_A: MAX96712 MFP2 TX_EN = 1, OUT_DIS = 1, COMP_EN = 0
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0307U, Data);
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0308U, 0x40);
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x031CU, 0x83U); //GPIO_A: MAX96712 MFP9 TX_EN = 1, OUT_DIS = 1, COMP_EN = 0

                if (EnabledLinkID == 0x1U) {
                    RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x03AAU, 0x22U); //[TBD] set for 1ch
                }

                /* SER_MFPx(GPIOx) as output and source enabled for GMSL2 reception */
                Data = 0x40U | pSerdesConfig->FrameSyncSel.DesGpio;
                RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, (0x2BEU + ((UINT16) pSerdesConfig->FrameSyncSel.SerGpio * 3U)), 0x84);
                RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, (0x2BFU + ((UINT16) pSerdesConfig->FrameSyncSel.SerGpio * 3U)), 0xb0);
                RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, (0x2C0U + ((UINT16) pSerdesConfig->FrameSyncSel.SerGpio * 3U)), Data);/* RX ID */
            }

            // -----------------------------------------------------------
            // Serializer MIPI Setup
            // -----------------------------------------------------------
            /* Set MIPI_RX registers number of lanes 1x4 mode */
            (void) Max96717_96722_RegWrite(VinID, SerAddr, 0x0330, 0x00);
            /* Select number of data lanes for Port B. */
            Data = (UINT8)((pSerdesConfig->CSIRxLaneNum - 1U) << 4U);
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0331, Data);
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0332, 0xEE);
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0333, 0xE4);

            // -----------------------------------------------------------
            //  sensor I2C tanslation
            // -----------------------------------------------------------
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);

            // -----------------------------------------------------------
            //  Customized: MFP4 & MPF7 faster slew rate
            // -----------------------------------------------------------
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0570, 0x1C);
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0571, 0xEC);
        }
        if ((EnabledLinkID & 0x2U) != 0U) {
            SerAddr = pCurSerDesInfo->SerAddr1;
            SensorAliasID = pSerdesConfig->SensorAliasID[1];

            /* SER96717: MIPI Port B -> Pipe Z (TX Stream ID=2) */
            Data = 0x20U | (pSerdesConfig->NumDataBits & 0x1fU);
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x031e, Data); /* Software override enable for Video Pipe Z */
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0308, 0x64); /* Select CSI port B for Video Pipe Z */
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0311, 0x40); /* Start Video Pipe Z from CSI port B */
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x005B, 0x02); /* Set Video Pipe Z Stream ID = 2*/

            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0383, 0x00);
            Data = 0x40U | pSerdesConfig->DataType;
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0318, Data); /*  RAW to pipe Z */

            if (pSerdesConfig->EnableExtFrameSync> 0U) {
                // -----------------------------------------------------------
                // GPIO forwarding setup
                // -----------------------------------------------------------
                /* DES_MFPx(GPIOx) as input and source enabled for GMSL2 transmission */
                Data = 0x40U | pSerdesConfig->FrameSyncSel.DesGpio;
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x04A0U, 0x08); //FSYNC_MODE = Frame sync generation is off, GPIO is used for FSYNC, type = GMSL2
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x04AFU, 0x9F);
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0306U, 0x83); //GPIO_A: MAX96712 MFP2 TX_EN = 1, OUT_DIS = 1, COMP_EN = 0
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x031CU, 0x83); //GPIO_A: MAX96712 MFP9 TX_EN = 1, OUT_DIS = 1, COMP_EN = 0

                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x033DU, 0x22);
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x030EU, Data);
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x030FU, 0x40);
                RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0354U, 0x29); //[TBD] r0x0374U=0x22: 1ch/IMX390

                /* SER_MFPx(GPIOx) as output and source enabled for GMSL2 reception */
                Data = 0x40U | pSerdesConfig->FrameSyncSel.DesGpio;
                RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, (0x2BEU + ((UINT16) pSerdesConfig->FrameSyncSel.SerGpio * 3U)), 0x84);
                RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, (0x2BFU + ((UINT16) pSerdesConfig->FrameSyncSel.SerGpio * 3U)), 0xb0);
                RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, (0x2C0U + ((UINT16) pSerdesConfig->FrameSyncSel.SerGpio * 3U)), Data);
            }

            // -----------------------------------------------------------
            // Serializer MIPI Setup
            // -----------------------------------------------------------
            /* Set MIPI_RX registers number of lanes 1x4 mode */
            (void) Max96717_96722_RegWrite(VinID, SerAddr, 0x0330, 0x00);
            /* Select number of data lanes for Port B. */
            Data = (UINT8)((pSerdesConfig->CSIRxLaneNum - 1U) << 4U);
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0331, Data);
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0332, 0xEE);
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0333, 0xE4);

            // -----------------------------------------------------------
            //  sensor I2C tanslation
            // -----------------------------------------------------------
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);

            // -----------------------------------------------------------
            //  Customized: MFP4 & MPF7 faster slew rate
            // -----------------------------------------------------------
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0570, 0x1C);
            RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0571, 0xEC);
        }

        // CSI bit rate per lane, CSITxSpeed * 100 MHz
        Data = 0x20U | pSerdesConfig->CSITxSpeed[0];
        RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0415U, Data);
        RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0418U, Data);
        RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x041BU, Data);
        RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x041EU, Data);

        if (EnabledLinkID == 0x1U) {
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0973, 0x10);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0100, 0x22);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0106, 0x0A);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x00F0, 0x62);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x00F4, 0x01);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x08A3, 0xE4);

            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x08A4, 0xE4);

            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x090B, 0x07);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x092D, 0x15);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x090D, 0x2D);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x090E, 0x2D);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x090F, 0x00);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0910, 0x00);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0911, 0x01);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0912, 0x01);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x08A0, 0x04);
        }

        if (EnabledLinkID == 0x2U) {
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0973, 0x10);
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0100, 0x22);
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0106, 0x0A);
            // 0x04,0x52,0x01,0x12,0x22,     // Disable sequence miss on pipe 1
            // 0x04,0x52,0x01,0x18,0x0A,    // Limit heartbeat on pipe 1
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0112, 0x22);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0118, 0x0A);

            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x00F0, 0x62);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x00F4, 0x02);

            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x08A3, 0xE4);

            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x08A4, 0xE4);

            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x090B, 0x07);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x092D, 0x15);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x090D, 0x2D);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x090E, 0x2D);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x090F, 0x00);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0910, 0x00);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0911, 0x01);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0912, 0x01);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x08A0, 0x04);
        }

        if (EnabledLinkID == 0x3U) {
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0973, 0x10);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0100, 0x23);
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0106, 0x0A);
            //0x04,0x52,0x01,0x12,0x22,     // Disable sequence miss on pipe 1
            //0x04,0x52,0x01,0x18,0x0A,    // Limit heartbeat on pipe 1
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0112, 0x23);
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0118, 0x0A);

            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x00F0, 0x62);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x00F4, 0x03);

            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x08A3, 0xE4);

            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x08A4, 0xE4);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0971U, 0x03);

            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x090B, 0x07);
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x092D, 0x15);
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x090D, 0x2D);
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x090E, 0x2D);
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x090F, 0x00);
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0910, 0x00);
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0911, 0x01);
            // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0912, 0x01);
            RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x08A0, 0x04);
        }

        // Enable MAX96722 MIPI CSI
        RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x040B, 0x02);  //>>

        // -----------------------------------------------------------
        // Enable TUNNELING mode
        // -----------------------------------------------------------
        // Data = 0x80U;
        // RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0383, Data);
        // Data = 0x09U;
        // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x0474, Data);
        // Data = 0x0FU;
        // RetVal |= Max96717_96722_RegWrite(VinID, DesAddr, 0x04B4, Data);
        // -----------------------------------------------------------
        // Disable TUNNELING mode
        // -----------------------------------------------------------
        // Data = 0x00U;
        // RetVal |= Max96717_96722_RegWrite(VinID, SerAddr, 0x0383, Data);
        // Data = 0x09U;
        // RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x0474, Data);
        // Data = 0x0FU;
        // RetVal |= Max96717_96716_RegWrite(VinID, DesAddr, 0x04B4, Data);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96722_SetSensorClk
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

UINT32 Max96717_96722_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkOut)
{
    UINT32 RetVal = MAX96717_96722_ERR_NONE;
    UINT8 SlaveID = 0x0U;
    const MAX96717_96722_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX96717_96722_ERR_INVALID_API;
    } else {
        if (ChipID == MAX96717_A_ID) {
            SlaveID = pCurSerDesInfo->SerAddr0;
        } else if (ChipID == MAX96717_B_ID) {
            SlaveID = pCurSerDesInfo->SerAddr1;
        } else {
            RetVal = MAX96717_96722_ERR_INVALID_API;
        }

        if (RetVal == MAX96717_96722_ERR_NONE) {
            if (RClkOut == MAX96717_96722_CLKFREQ_25M_MFP2) {
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x0003, 0x04); /* MFP2 to output RCLKOUT XTAL/1=25MHz */
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x0006, 0xBF); /* enable RCLK output */

            } else if (RClkOut == MAX96717_96722_CLKFREQ_24M_MFP2) {
                /* pre-defined clock setting is disabled */
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x03F0, 0x02);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x03F4, 0x0A);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x03F5, 0x07);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x03F0, 0x00);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x1A03, 0x10);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x1A07, 0x04);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x1A08, 0x3D);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x1A09, 0x40);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x1A0A, 0x40);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x1A0B, 0x60);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x03F0, 0x01);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x0003, 0x07);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x0006, 0xBF);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x0570, 0x00);

            } else if (RClkOut == MAX96717_96722_CLKFREQ_27M_MFP2) {
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x03f0, 0x02); /* reset reference generation PLL */
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x03f0, 0x51); /* enable pre-defined clock 27Mhz */
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x0003, 0x07); /* MFP2 to output RCLKOUT */
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x056f, 0x0e); /* MFP2 rise/fall time speed setting */
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x0006, 0xBF); /* enable RCLK output */

            } else if (RClkOut == MAX96717_96722_CLKFREQ_25M_MFP4) {
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x0003, 0x00); /* MFP4 to output RCLKOUT XTAL/1=25MHz */
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x0006, 0xBF); /* enable RCLK output */

            } else if (RClkOut == MAX96717_96722_CLKFREQ_24M_MFP4) {
                /* pre-defined clock setting is disabled */
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x03F0, 0x02);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x03F4, 0x0A);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x03F5, 0x07);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x03F0, 0x00);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x1A03, 0x10);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x1A07, 0x04);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x1A08, 0x3D);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x1A09, 0x40);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x1A0A, 0x40);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x1A0B, 0x60);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x03F0, 0x01);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x0003, 0x03);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x0006, 0xBF);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, 0x0570, 0x00);
            } else {
                RetVal = MAX96717_96722_ERR_ARG;
            }
        }
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96722_SetGpioOutput
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
UINT32 Max96717_96722_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT8 Level)
{
    UINT32 RetVal = MAX96717_96722_ERR_NONE;
    UINT16 Addr = 0U; /* register address */
    MAX96717_96722_GPIO_A_REG_s GpioAReg = {0};
    MAX96717_96722_GPIO_B_REG_s GpioBReg = {0};
    const MAX96717_96722_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT8 SlaveID = 0U, RegAVal, RegBVal;

    if (ChipID == MAX96722_ID) {
        Addr = (UINT16)0x2b0 + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->DesAddr;
    } else if (ChipID == MAX96717_A_ID) {
        Addr = (UINT16)0x2be + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->SerAddr0;
    } else if (ChipID == MAX96717_B_ID) {
        Addr = (UINT16)0x2be + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->SerAddr1;
    } else {
        RetVal = MAX96717_96722_ERR_ARG;
    }
    if (RetVal == MAX96717_96722_ERR_NONE) {

        if (PinID >= MAX96717_96722_NUM_GPIO_PIN) {
            RetVal = MAX96717_96722_ERR_ARG;
        } else {
            RetVal |= Max96717_96722_RegRead(VinID, SlaveID, Addr, &RegAVal);
            RetVal |= Max96717_96722_RegRead(VinID, SlaveID, Addr + 1U, &RegBVal);
            if (RetVal == MAX96717_96722_ERR_NONE) {
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
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, Addr, RegAVal);
                RetVal |= Max96717_96722_RegWrite(VinID, SlaveID, Addr + 1U, RegBVal);
            } else {
                RetVal = MAX96717_96722_ERR_ARG;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96722_RegisterWrite
 *
 *  @Description:: Write Max96717_96722 registers API
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
UINT32 Max96717_96722_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 Data)
{
    UINT32 RetVal = MAX96717_96722_ERR_NONE;
    const MAX96717_96722_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX96717_96722_ERR_INVALID_API;
    } else {
        if (ChipID == MAX96722_ID) {
            RetVal = Max96717_96722_RegWrite(VinID, pCurSerDesInfo->DesAddr, Addr, Data);
        } else if (ChipID == MAX96717_A_ID) {
            RetVal = Max96717_96722_RegWrite(VinID, pCurSerDesInfo->SerAddr0, Addr, Data);
        } else if (ChipID == MAX96717_B_ID) {
            RetVal = Max96717_96722_RegWrite(VinID, pCurSerDesInfo->SerAddr1, Addr, Data);
        } else {
            RetVal = MAX96717_96722_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max96717_96722_RegisterRead
 *
 *  @Description:: Read Max96717_96722 registers API
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
UINT32 Max96717_96722_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT8 Addr, UINT8 *pData)
{
    UINT32 RetVal = MAX96717_96722_ERR_NONE;
    const MAX96717_96722_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX96717_96722_ERR_INVALID_API;
    } else {
        if (ChipID == MAX96722_ID) {
            RetVal = Max96717_96722_RegRead(VinID, pCurSerDesInfo->DesAddr, Addr, pData);
        } else if (ChipID == MAX96717_A_ID) {
            RetVal = Max96717_96722_RegRead(VinID, pCurSerDesInfo->SerAddr0, Addr, pData);
        } else if (ChipID == MAX96717_B_ID) {
            RetVal = Max96717_96722_RegRead(VinID, pCurSerDesInfo->SerAddr1, Addr, pData);
        } else {
            RetVal = MAX96717_96722_ERR_ARG;
        }
    }

    return RetVal;
}
