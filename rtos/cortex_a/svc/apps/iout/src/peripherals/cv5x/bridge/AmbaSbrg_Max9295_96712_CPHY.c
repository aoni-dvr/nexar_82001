/**
 *  @file AmbaSbrg_Max9295_96712.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details MAX9295 & MAX96712 driver APIs
 *
 */

//#include "AmbaSSP.h"

#include "AmbaKAL.h"
#include "AmbaPrint.h"

#include "AmbaI2C.h"
#include "AmbaVIN.h"
#include "AmbaSbrg_Max9295_96712.h"

#include "bsp.h"

#define MAX9295_96712_DEBUG //enable if when need to print all sensor setting
#define REDUCE_BOOT_TIME    //reduce delay time during Maxim setting which not confirm by FAE
#define SERIALIZER_BC       //Serializer BC
//#define MAX9295_96712_FULL_LOG          //use Print_Flush to get full log for debug only since it increases much time
#define MX01_CPHY
#define VCX

typedef struct {
    AMBA_KAL_MUTEX_t Mutex;
    UINT32 InitDone;
    UINT8 DesAddr;          /* Max96712 I2C Slave Addr */
    UINT8 SerAddr0;         /* Distinguishable Max9295 LinkA I2C Slave Addr */
    UINT8 SerAddr1;         /* Distinguishable Max9295 LinkB I2C Slave Addr */
    UINT8 SerAddr2;         /* Distinguishable Max9295 LinkC I2C Slave Addr */
    UINT8 SerAddr3;         /* Distinguishable Max9295 LinkD I2C Slave Addr */
} MAX9295_96712_SERDES_INFO_s;

static UINT32 MAX9295_96712I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
#ifndef MX01_CPHY
    [AMBA_VIN_CHANNEL0]  = AMBA_MAXIM_I2C_CHANNEL,
#else
    [AMBA_VIN_CHANNEL0]  = AMBA_MAXIM_I2C_CHANNEL_VIN4,
#endif
    [AMBA_VIN_CHANNEL1]  = AMBA_MAXIM_I2C_CHANNEL_VIN1,
    [AMBA_VIN_CHANNEL2]  = AMBA_MAXIM_I2C_CHANNEL_VIN2,
    [AMBA_VIN_CHANNEL3]  = AMBA_MAXIM_I2C_CHANNEL_VIN3,
#ifndef MX01_CPHY
    [AMBA_VIN_CHANNEL4]  = AMBA_MAXIM_I2C_CHANNEL_VIN4,
#else
    [AMBA_VIN_CHANNEL4]  = AMBA_MAXIM_I2C_CHANNEL,
#endif
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

static MAX9295_96712_SERDES_INFO_s MaxSerDesInfo[AMBA_NUM_VIN_CHANNEL];

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_RegWrite
 *
 *  @Description:: Write Max9295_96712 registers
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
static UINT32 Max9295_96712_RegWrite(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 Data)
{
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};

    UINT8 TxDataBuf[3];
    UINT32 TxSize;

    I2cConfig.SlaveAddr = SlaveID;
    I2cConfig.DataSize  = 3U;
    I2cConfig.pDataBuf  = TxDataBuf;

    TxDataBuf[0] = (UINT8) (Addr >> 8U);
    TxDataBuf[1] = (UINT8) (Addr & 0xffU);
    TxDataBuf[2] = Data;

    RetVal |= AmbaI2C_MasterWrite(MAX9295_96712I2cChannel[VinID], AMBA_I2C_SPEED_FAST, &I2cConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "I2C does not work! WIB %02X %04X %02X", SlaveID, Addr, Data, 0U, 0U);
        RetVal = MAX9295_96712_ERR_COMMUNICATE;
    }
#ifdef MAX9295_96712_DEBUG
    /* print using MAXIM GMSL GUI's write command format */
    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] I2C_Ch %d WIB %02X %04X %02X", MAX9295_96712I2cChannel[VinID], SlaveID, Addr, Data, 0U);
#ifdef MAX9295_96712_FULL_LOG
    AmbaPrint_Flush();
#endif
#endif

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
static UINT32 Max9295_96712_RegRead(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = SlaveID;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8) (Addr >> 8U);
    TxData[1] = (UINT8) (Addr & 0xffU);

    I2cRxConfig.SlaveAddr = (UINT32)SlaveID | (UINT32)1U;
    I2cRxConfig.DataSize  = 1U;
    I2cRxConfig.pDataBuf  = pRxData;

    RetVal |= AmbaI2C_MasterReadAfterWrite(MAX9295_96712I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD, 1U,
                                           &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "SlaveID 0x%02x Addr 0x%04x I2C does not work!!!!!", SlaveID, Addr, 0U, 0U, 0U);
        RetVal = MAX9295_96712_ERR_COMMUNICATE;
    }

    return RetVal;
}

static UINT32 Max9295_96712_RegReadModifyWrite(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 Data, UINT8 Mask)
{
    UINT32 RetVal;
    UINT8 RData;

    RetVal = Max9295_96712_RegRead(VinID, SlaveID, Addr, &RData);

    if (RetVal == MAX9295_96712_ERR_NONE) {
        RetVal = Max9295_96712_RegWrite(VinID, SlaveID, Addr, Data | (RData & ~Mask));
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
static UINT32 Max9295_96712_WaitLockStatus(UINT32 VinID, UINT8 SlaveID, UINT32 TimeOut, UINT16 EnabledLinkID)
{
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    UINT32 RetValTmp = 0U;
    UINT32 Count = 0U;
    UINT8 Count_ary[4], RxData[4];
    UINT8 Lock = 0U; //4'b1111, 1: Lock OK, 0: Lock NG

    RxData[0] = 0;
    RxData[1] = 0;
    RxData[2] = 0;
    RxData[3] = 0;
    Count_ary[0] = 0;
    Count_ary[1] = 0;
    Count_ary[2] = 0;
    Count_ary[3] = 0;

    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Please ignore \"I2C does not work\" msg before locked to wait for i2c communication", RxData[0], Count_ary[0], 0U, 0U, 0U);
    while ((Lock != EnabledLinkID) && (Count != TimeOut)) {

        if(((EnabledLinkID & 0x1U) == 0x1U) && ((Lock & 0x1U) != 0x1U)) { //LinkA
            RetValTmp = Max9295_96712_RegRead(VinID, SlaveID, 0x001aU, &RxData[0]);
            Count_ary[0] = (UINT8)Count;

            if ((RetValTmp == OK) && ((RxData[0] & 0x08U) == 0x08U)) {
                Lock |= 0x1U;
            }
        }

        if(((EnabledLinkID & 0x2U) == 0x2U) && ((Lock & 0x2U) != 0x2U)) { //LinkB
            RetValTmp = Max9295_96712_RegRead(VinID, SlaveID, 0x000aU, &RxData[1]);
            Count_ary[1] = (UINT8)Count;

            if ((RetValTmp == OK) && ((RxData[1] & 0x08U) == 0x08U)) {
                Lock |= 0x2U;
            }
        }

        if(((EnabledLinkID & 0x4U) == 0x4U) && ((Lock & 0x4U) != 0x4U)) { //LinkC
            RetValTmp = Max9295_96712_RegRead(VinID, SlaveID, 0x000bU, &RxData[2]);
            Count_ary[2] = (UINT8)Count;

            if ((RetValTmp == OK) && ((RxData[2] & 0x08U) == 0x08U)) {
                Lock |= 0x4U;
            }
        }

        if(((EnabledLinkID & 0x8U) == 0x8U) && ((Lock & 0x8U) != 0x8U)) { //LinkD
            RetValTmp = Max9295_96712_RegRead(VinID, SlaveID, 0x000cU, (&RxData[3]));
            Count_ary[3] = (UINT8)Count;

            if ((RetValTmp == OK) && ((RxData[3] & 0x08U) == 0x08U)) {
                Lock |= 0x8U;
            }
        }

        (void)AmbaKAL_TaskSleep(1U);
        Count++;
    }

    //AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[Max9295_96712_WaitLockStatus] Lock 0x%02x", Lock, 0U, 0U, 0U, 0U);
    //LinkA msg
    if((EnabledLinkID & 0x1U) != 0U) {
        if((Lock & 0x1U) != 0U) {
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Max96712 LinkA Reg0x1a=0x%02x (%d) - GMSL2 locked!", RxData[0], Count_ary[0], 0U, 0U, 0U);
        } else {
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Max96712 LinkA Reg0x1a=0x%02x (%d) - GMSL2 NOT locked!", RxData[0], Count_ary[0], 0U, 0U, 0U);
            RetVal = MAX9295_96712_ERR_SERDES_LINK;
        }
    }
    //LinkB msg
    if((EnabledLinkID & 0x2U) != 0U) {
        if((Lock & 0x2U) != 0U) {
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Max96712 LinkB Reg0x0a=0x%02x (%d) - GMSL2 locked!", RxData[1], Count_ary[1], 0U, 0U, 0U);
        } else {
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Max96712 LinkB Reg0x0a=0x%02x (%d) - GMSL2 NOT locked!", RxData[1], Count_ary[1], 0U, 0U, 0U);
            RetVal = MAX9295_96712_ERR_SERDES_LINK;
        }
    }
    //LinkC msg
    if((EnabledLinkID & 0x4U) != 0U) {
        if((Lock & 0x4U) != 0U) {
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Max96712 LinkC Reg0x0b=0x%02x (%d) - GMSL2 locked!", RxData[2], Count_ary[2], 0U, 0U, 0U);
        } else {
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Max96712 LinkC Reg0x0b=0x%02x (%d) - GMSL2 NOT locked!", RxData[2], Count_ary[2], 0U, 0U, 0U);
            RetVal = MAX9295_96712_ERR_SERDES_LINK;
        }
    }
    //LinkD msg
    if((EnabledLinkID & 0x8U) != 0U) {
        if((Lock & 0x8U) != 0U) {
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Max96712 LinkD Reg0x0c=0x%02x (%d) - GMSL2 locked!", RxData[3], Count_ary[3], 0U, 0U, 0U);
        } else {
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Max96712 LinkD Reg0x0c=0x%02x (%d) - GMSL2 NOT locked!", RxData[3], Count_ary[3], 0U, 0U, 0U);
            RetVal = MAX9295_96712_ERR_SERDES_LINK;
        }
    }
    return RetVal;
}


UINT32 Max9295_96712_VCC_Init(UINT32 VinID, UINT16 EnabledLinkID)
{
    MAX9295_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    UINT8 Data, Data1;

#ifndef REDUCE_BOOT_TIME
    AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "[Max9295_96712_Init] Normal Boot Time", NULL, NULL, NULL, NULL, NULL);
#else
    AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "[Max9295_96712_Init] Reduce Boot Time", NULL, NULL, NULL, NULL, NULL);
#endif

#ifdef MAX9295_96712_FULL_LOG
    AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "[Max9295_96712] Enable Full log to debug only. The increased time is normal.", NULL, NULL, NULL, NULL, NULL);
#endif

    if (pCurSerDesInfo->InitDone == 0U) {

        /* Create a mutex */
        RetVal = AmbaKAL_MutexCreate(&pCurSerDesInfo->Mutex, NULL);
        if (RetVal != MAX9295_96712_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "[Max9295_96712] AmbaKAL_MutexCreate() failed!", NULL, NULL, NULL, NULL, NULL);
        }
    } else {
        //InitDone = 1U
    }
    pCurSerDesInfo->DesAddr = MX01_IDC_ADDR_MAX96712;
    pCurSerDesInfo->SerAddr0 = MX01_IDC_ADDR_MAX9295_A;
    pCurSerDesInfo->SerAddr1 = MX01_IDC_ADDR_MAX9295_B;
    pCurSerDesInfo->SerAddr2 = MX01_IDC_ADDR_MAX9295_C;
    pCurSerDesInfo->SerAddr3 = MX01_IDC_ADDR_MAX9295_D;
    pCurSerDesInfo->InitDone = 1U;

    //Check locked
#ifndef REDUCE_BOOT_TIME
    (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
#endif
    RetVal = Max9295_96712_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200U, EnabledLinkID);

    //Check MAX96712 version
    if((Max9295_96712_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0d, &Data) == I2C_ERR_NONE)) {
        if(Max9295_96712_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x4c, &Data1) == I2C_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Check MAX96712 Chip version r0xd=0x%x, r0x4c=0x%x", Data, Data1, 0U, 0U, 0U);
            if((Data == 0xA0U) && ((Data1 == 0U) || (Data1 == 1U) || (Data1 == 2U) || (Data1 == 3U) )) {
                if(Data1 == 0x2U) {
                    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Supported ES2.0 MAX96712", 0U, 0U, 0U, 0U, 0U);
                } else if(Data1 == 0x3U) {
                    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Supported Production Samples MAX96712(new), remove phy optimization to check the failed issue", 0U, 0U, 0U, 0U, 0U);

                    //PHYA/PHB/PHYC/PHYD optimization for Productions Sample by FAE's suggestion(19/06)
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1458, 0x28);
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1459, 0x68);
                    //FAE add
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x143E, 0xB3);
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x143F, 0x72);

                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1558, 0x28);
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1559, 0x68);
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x153E, 0xB3);
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x153F, 0x72);

                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1658, 0x28);
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1659, 0x68);
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x163E, 0xB3);
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x163F, 0x72);

                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1758, 0x28);
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1759, 0x68);
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x173E, 0xB3);
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x173F, 0x72);

#ifndef REDUCE_BOOT_TIME
                    //do one reset and delay 25ms to check status
                    RetVal |= Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0018U, 0x0FU);
                    (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
                    RetVal |= Max9295_96712_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200, EnabledLinkID);
#endif

                } else {
                    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Unknown version MAX96712", 0U, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Not in MAX96712 Supported List", 0U, 0U, 0U, 0U, 0U);
            }
        } else {
            (void)Max9295_96712_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x4c, &Data1);
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Check MAX96712 Chip version r0xd=0x%x, r0x4c=0x%x", Data, Data1, 0U, 0U, 0U);
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Not in MAX96712 Supported List", 0U, 0U, 0U, 0U, 0U);
        }
    }

    AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "[Max9295_96712] Please ignore i2c failed for 1st bootup or not connect to those MAX9295", NULL, NULL, NULL, NULL, NULL);

    //if modified MAX9295 i2c slave addr existed =>  reset MAX9295 for all (not first boot-up case)
    if(Max9295_96712_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0000U, &Data) == MAX9295_96712_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "[Max9295_96712] reset MAX9295 LinkA for not power off/on case", NULL, NULL, NULL, NULL, NULL);

        ///set MFP2 as GPIO low to make CFG1 works fine when reset MAX9295
        //disable MAX9295 clk_si
        (void) Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x03, 0x3U);
        //set MFP2 as GPIO low
        (void) Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x2c4, 0x80U);
        //MAX9295 reset all
        (void) Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x10, 0x81U); //TBD: 0x81 for LinkA only by FAE

    } else {
        //not reset LinkA Max9295
    }

    if(Max9295_96712_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0000U, &Data) == MAX9295_96712_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "[Max9295_96712] reset MAX9295 LinkB for not power off/on case", NULL, NULL, NULL, NULL, NULL);
        ///set MFP2 as GPIO low to make CFG1 works fine when reset MAX9295
        //disable MAX9295 clk_si
        (void) Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x03, 0x3U);
        //set MFP2 as GPIO low
        (void) Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x2c4, 0x80U);
        //MAX9295 reset all
        (void) Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x10, 0x81U); //TBD: 0x81 for LinkA only by FAE
    } else {
        //not reset LinkB Max9295
    }

    if(Max9295_96712_RegRead(VinID, pCurSerDesInfo->SerAddr2, 0x0000U, &Data) == MAX9295_96712_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "[Max9295_96712] reset MAX9295 LinkC for not power off/on case", NULL, NULL, NULL, NULL, NULL);
        ///set MFP2 as GPIO low to make CFG1 works fine when reset MAX9295
        //disable MAX9295 clk_si
        (void) Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr2, 0x03, 0x3U);
        //set MFP2 as GPIO low
        (void) Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr2, 0x2c4, 0x80U);
        //MAX9295 reset all
        (void) Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr2, 0x10, 0x81U); //TBD: 0x81 for LinkA only by FAE
    } else {
        //not reset LinkB Max9295
    }

    if(Max9295_96712_RegRead(VinID, pCurSerDesInfo->SerAddr3, 0x0000U, &Data) == MAX9295_96712_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "[Max9295_96712] reset MAX9295 LinkD for not power off/on case", NULL, NULL, NULL, NULL, NULL);
        ///set MFP2 as GPIO low to make CFG1 works fine when reset MAX9295
        //disable MAX9295 clk_si
        (void) Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr3, 0x03, 0x3U);
        //set MFP2 as GPIO low
        (void) Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr3, 0x2c4, 0x80U);
        //MAX9295 reset all
        (void) Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr3, 0x10, 0x81U); //TBD: 0x81 for LinkA only by FAE
    } else {
        //not reset LinkB Max9295
    }
#ifndef REDUCE_BOOT_TIME
    //Check locked
    (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
    RetVal |= Max9295_96712_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200, EnabledLinkID);
#endif
#if 0
    // -----------------------------------------------------------
    // Reset parts
    // -----------------------------------------------------------
    // One-shot reset is only needed if you change operation mode, for example link rate.
    // Or if you see something abnormal then one-shot may fix most of sequencing issues.
    // By default, MAX96712A is already in single link mode. You don?™t need to one-sho
    // reset unless you change it to other mode for example splitter mode.
    AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] one-shot reset", EnabledLinkID);
    Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x31U);

    if (Max9295_96712_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 100U) != OK)
        return NG;
#endif

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_GetSerdesStatus
 *
 *  @Description:: Max9295_9296 Get Serdes/Sensor video/link Status API
 *
 *  @Input      ::
 *      VinID:             VIN ID
 *      EnabledLinkID:  EnabledLinkID
 *      pSerdesStatus:  Pointer to serdes/sensor video/link status
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max9295_96712_VCC_GetSerdesStatus(UINT32 VinID, UINT16 EnabledLinkID, MAX9295_96712_SERDES_STATUS_s *pSerdesStatus)
{
    UINT8 RxData = 0;
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    UINT16 i, j;
    static const UINT16 LinkLockAddr[4] = {0x001aU, 0x000aU, 0x000bU, 0x000cU};
    static const UINT16 VideoLockAddr[4] = {0x0108U, 0x011aU, 0x012cU, 0x013eU};


    if ((EnabledLinkID & 0xFU) == 0U) {
        RetVal = ERR_ARG;
    } else {
        j = 0U;

        for (i=0U; i < MAX96712_NUM_RX_PORT; i ++) {

            if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
                //LinkLock
                if (Max9295_96712_RegRead(VinID, MX01_IDC_ADDR_MAX96712, LinkLockAddr[i], &RxData) != MAX9295_96712_ERR_NONE) {
                    pSerdesStatus[i].LinkLock = MX01_SERDES_STATUS_NOT_SUPPORTED;
                    pSerdesStatus[i].VideoLock = MX01_SERDES_STATUS_NOT_SUPPORTED;
                    RetVal |= MAX9295_96712_ERR_COMMUNICATE;
                } else {
                    if((RxData & 0x08U) == 0x08U) {
                        pSerdesStatus[i].LinkLock = MX01_SERDES_STATUS_OK;
                    } else {
                        pSerdesStatus[i].LinkLock = MX01_SERDES_STATUS_NG;
                    }
                    //VideoLock
                    if(Max9295_96712_RegRead(VinID, MX01_IDC_ADDR_MAX96712, VideoLockAddr[j], &RxData) != MAX9295_96712_ERR_NONE) {
                        pSerdesStatus[i].VideoLock = MX01_SERDES_STATUS_NOT_SUPPORTED;
                    } else {
                        if((RxData & 0x40U) == 0x40U) {
                            pSerdesStatus[i].VideoLock = MX01_SERDES_STATUS_OK;
                        } else {
                            pSerdesStatus[i].VideoLock = MX01_SERDES_STATUS_NG;
                        }
                    }
                }
                j ++;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_Config
 *
 *  @Description:: Configure Max9295_96712
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
UINT32 Max9295_96712_VCC_Config(UINT32 VinID, const MAX9295_96712_SERDES_CONFIG_s *pSerdesConfig)
{
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    MAX9295_96712_SERDES_CONFIG2_s SerdesConfig2 = {0};
    UINT32 i;

    if (pSerdesConfig == NULL) {
        RetVal = MAX9295_96712_ERR_ARG;
    } else {
        SerdesConfig2.EnabledLinkID = pSerdesConfig->EnabledLinkID;
        SerdesConfig2.SensorBCID = pSerdesConfig->SensorBCID;

        for (i = 0; i < MAX96712_NUM_RX_PORT; i++) {
            SerdesConfig2.SensorSlaveID[i] = pSerdesConfig->SensorSlaveID;
            SerdesConfig2.SensorAliasID[i] = pSerdesConfig->SensorAliasID[i];
            SerdesConfig2.DataType[i] = pSerdesConfig->DataType;
            SerdesConfig2.DataType2[i] = pSerdesConfig->DataType2;
            SerdesConfig2.NumDataBits[i] = pSerdesConfig->NumDataBits;
            SerdesConfig2.CSIRxLaneNum[i] = pSerdesConfig->CSIRxLaneNum;
        }

        for (i = 0; i < MAX96712_NUM_TX_PORT; i++) {
            SerdesConfig2.CSITxLaneNum[i] = pSerdesConfig->CSITxLaneNum[i];
            SerdesConfig2.CSITxSpeed[i] = pSerdesConfig->CSITxSpeed[i];
        }

        SerdesConfig2.VideoAggregationMode = MAX9295_MAX96712_VIDEO_AGGR_LINE_INTERLEAVED;

        RetVal = Max9295_96712_Config2(VinID, &SerdesConfig2);
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_Config2
 *
 *  @Description:: Configure Max9295_96712
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
UINT32 Max9295_96712_VCC_Config2(UINT32 VinID, const MAX9295_96712_SERDES_CONFIG2_s *pSerdesConfig)
{
    const MAX9295_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    UINT16 EnabledLinkID;
    UINT8 SensorSlaveID, SensorAliasID, SensorBCID;
    UINT8 SerAddr, DesAddr;
    UINT8 Data;
    UINT32 i;
#ifdef SERIALIZER_BC
    UINT8 BCInit = 0; //1: record 1st Config Chan value
    UINT8 BCCSIRxLaneNum = 0U;
    UINT8 BCDataType = 0U;
    UINT8 BCDataType2 = 0U;
    UINT16 BCFlag[3]; //BCFlag[]=0 when CSIRxLaneNum/DataType/DataType2 is the same for each Chan

    //init BCFlag
    for (i=0; i < 3U; i++) {
        BCFlag[i] = 0U;
    }
#endif

    if (pSerdesConfig == NULL) {
        RetVal = MAX9295_96712_ERR_ARG;
    } else {
        if (pCurSerDesInfo->InitDone == 0U) {
            RetVal = MAX9295_96712_ERR_INVALID_API;
        } else {
            EnabledLinkID = pSerdesConfig->EnabledLinkID;
            DesAddr = pCurSerDesInfo->DesAddr;
            SensorBCID = pSerdesConfig->SensorBCID;

#ifndef MX01_CPHY
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] MIPI DPHY", 0U, 0U, 0U, 0U, 0U);
#else
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] MIPI CPHY", 0U, 0U, 0U, 0U, 0U);
#endif
            if (pSerdesConfig->VideoAggregationMode != MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                if((EnabledLinkID == 0x3U) || (EnabledLinkID == 0x7U) || (EnabledLinkID == 0xbU) || (EnabledLinkID == 0xfU)) {
                    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Config (EnabledLinkID: 0x%02x)... transfer RAW12 only(multi-ch)", EnabledLinkID, 0U, 0U, 0U, 0U);
                } else if(EnabledLinkID == 0x1U) {
                    if(pSerdesConfig->DataType2[0] != 0xffU) {
                        AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Config (EnabledLinkID: 0x%02x)... transfer RAW%d+OB (1-ch)", EnabledLinkID, pSerdesConfig->NumDataBits[0], 0U, 0U, 0U);
                    } else {
                        AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Config (EnabledLinkID: 0x%02x)... transfer RAW%d only(1-ch)", EnabledLinkID, pSerdesConfig->NumDataBits[0], 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] NG! Incorrect SensorID 0x%02x", EnabledLinkID, 0U, 0U, 0U, 0U);
                    RetVal = MAX9295_96712_ERR_ARG;
                }
            }
#ifdef REDUCE_BOOT_TIME
            if (EnabledLinkID == 1U) {
#endif
                AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; GMSL2 mode for all links and only Link EnabledLinkID", NULL, NULL, NULL, NULL, NULL);
                //GMSL2 mode for all links and only Link by EnabledLinkID
                Data = (UINT8)(0xF0U + (EnabledLinkID & 0xFU));
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, Data);
                //Set 6Gbps link Rx rate for GMSL2
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0010U, 0x22U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0011U, 0x22U);
                //One-shot link reset for EnabledLinkID
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0018U, 0x0FU);
                (void)AmbaKAL_TaskSleep(100U);
                //Check locked
                (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
                RetVal |= Max9295_96712_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200, EnabledLinkID);
#ifdef REDUCE_BOOT_TIME
            } else {
                //
            }
#endif

            //I2C slave address translation

            AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; I2C slave address translation", NULL, NULL, NULL, NULL, NULL);
            //LinkA
            if ((EnabledLinkID & 0x1U) != 0U) {
                //Enable Link A Only and change Link A MAX9295A slave address to 0x82
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, 0xF1U);
#ifndef REDUCE_BOOT_TIME
                (void)AmbaKAL_TaskSleep(100U);
#else
                (void)AmbaKAL_TaskSleep(45U);
#endif
                RetVal |= Max9295_96712_RegWrite(VinID, MX01_IDC_ADDR_MAX9295_DEFAULT, 0x0000U, pCurSerDesInfo->SerAddr0);

            }
            //LinkB
            if ((EnabledLinkID & 0x2U) != 0U) {
                //Enable Link B Only and change Link B MAX9295A slave address to 0x84
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, 0xF2U);
#ifndef REDUCE_BOOT_TIME
                (void)AmbaKAL_TaskSleep(100U);
#else
                (void)AmbaKAL_TaskSleep(45U);
#endif
                RetVal |= Max9295_96712_RegWrite(VinID, MX01_IDC_ADDR_MAX9295_DEFAULT, 0x0000U, pCurSerDesInfo->SerAddr1);

            }
            //LinkC
            if ((EnabledLinkID & 0x4U) != 0U) {
                //TBD: FAE doesn't provide setting yet
                //Enable Link C Only and change Link C MAX9295A slave address to 0x86
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, 0xF4U);
#ifndef REDUCE_BOOT_TIME
                (void)AmbaKAL_TaskSleep(100U);
#else
                (void)AmbaKAL_TaskSleep(45U);
#endif
                RetVal |= Max9295_96712_RegWrite(VinID, MX01_IDC_ADDR_MAX9295_DEFAULT, 0x0000U, pCurSerDesInfo->SerAddr2);
            }
            //LinkD
            if ((EnabledLinkID & 0x8U) != 0U) {
                //Enable Link C Only and change Link C MAX9295A slave address to 0x88
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, 0xF8U);
#ifndef REDUCE_BOOT_TIME
                (void)AmbaKAL_TaskSleep(100U);
#else
                (void)AmbaKAL_TaskSleep(45U);
#endif
                RetVal |= Max9295_96712_RegWrite(VinID, MX01_IDC_ADDR_MAX9295_DEFAULT, 0x0000U, pCurSerDesInfo->SerAddr3);
            }
            if ((EnabledLinkID & 0xeU) != 0U) {
                AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; Re-enable multi-ch", NULL, NULL, NULL, NULL, NULL);
                Data = (UINT8)(0xF0U + (EnabledLinkID & 0xFU));
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, Data);
#ifndef REDUCE_BOOT_TIME
                (void)AmbaKAL_TaskSleep(100U);
#else
                (void)AmbaKAL_TaskSleep(45U);
#endif
            }

#ifdef SERIALIZER_BC
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] SERIALIZER_BC", 0U, 0U, 0U, 0U, 0U);
            //Serializer BC slave I2C
            if ((EnabledLinkID & 0x1U) != 0U) {
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x0042U, MX01_IDC_ADDR_MAX9295_BC);
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x0043U, pCurSerDesInfo->SerAddr0);
            } else {
                //
            }
            if ((EnabledLinkID & 0x2U) != 0U) {
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x0042U, MX01_IDC_ADDR_MAX9295_BC);
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x0043U, pCurSerDesInfo->SerAddr1);
            } else {
                //
            }
            if ((EnabledLinkID & 0x4U) != 0U) {
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr2, 0x0042U, MX01_IDC_ADDR_MAX9295_BC);
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr2, 0x0043U, pCurSerDesInfo->SerAddr2);
            } else {
                //
            }
            if ((EnabledLinkID & 0x8U) != 0U) {
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr3, 0x0042U, MX01_IDC_ADDR_MAX9295_BC);
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr3, 0x0043U, pCurSerDesInfo->SerAddr3);
            } else {
                //
            }
            SerAddr = MX01_IDC_ADDR_MAX9295_BC;

            for (i=0U; i < MAX96712_NUM_RX_PORT; i ++) {

                if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
                    if(BCInit == 0U) {
                        BCCSIRxLaneNum = pSerdesConfig->CSIRxLaneNum[i];
                        BCDataType = pSerdesConfig->DataType[i];
                        BCDataType2 = pSerdesConfig->CSIRxLaneNum[i];
                        BCInit = 1U;
                    } else {
                        if (BCFlag[0] == 0U) {
                            if (pSerdesConfig->CSIRxLaneNum[i] != BCCSIRxLaneNum) {
                                BCFlag[0] = 1U;
                            } else {
                                //
                            }
                        } else {
                            //
                        }
                        if (BCFlag[1] == 0U) {
                            if (pSerdesConfig->DataType[i] != BCDataType) {
                                BCFlag[1] = 1U;
                            } else {
                                //
                            }
                        } else {
                            //
                        }
                        if (BCFlag[2] == 0U)  {
                            if(pSerdesConfig->DataType2[i] != BCDataType2) {
                                BCFlag[2] = 1U;
                            } else {
                                //
                            }
                        } else {
                            //
                        }
                    }

                }
            }

            // --------------------------------------------------------------------------------------
            // MAX9295A powers up in GMSL2 mode
            // Config MIPI PHY / Controller
            // Set SER is in 1x4 mode; 4-lane in controller 1 for port A; Default lane mapping
            //----------------------------------------------------------------------------------------
            //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************         Link A: Serializer MAX9295A         *********************", NULL, NULL, NULL, NULL, NULL);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
            if(BCFlag[0] == 0U) {
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[0] - 1U) << 4U)); // select number of data lanes
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
            } else {
                if ((EnabledLinkID & 0x1U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr0;
                    Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[0] - 1U) << 4U)); // select number of data lanes
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                } else {
                    //
                }
                if ((EnabledLinkID & 0x2U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr1;
                    Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[1] - 1U) << 4U)); // select number of data lanes
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                } else {
                    //
                }
                if ((EnabledLinkID & 0x4U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr2;
                    Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[2] - 1U) << 4U)); // select number of data lanes
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                } else {
                    //
                }
                if ((EnabledLinkID & 0x8U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr3;
                    Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[3] - 1U) << 4U)); // select number of data lanes
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                } else {
                    //
                }
                SerAddr = MX01_IDC_ADDR_MAX9295_BC;
            }
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U);

            if(BCFlag[1U] == 0U) {
                Data = ((pSerdesConfig->DataType[0]) | 0x40U);  //bit[6] should be 1;
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);
            } else {
                if ((EnabledLinkID & 0x1U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr0;
                    Data = ((pSerdesConfig->DataType[0]) | 0x40U);  //bit[6] should be 1;
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);
                } else {
                    //
                }
                if ((EnabledLinkID & 0x2U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr1;
                    Data = ((pSerdesConfig->DataType[1]) | 0x40U);  //bit[6] should be 1;
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);
                } else {
                    //
                }
                if ((EnabledLinkID & 0x4U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr2;
                    Data = ((pSerdesConfig->DataType[2]) | 0x40U);  //bit[6] should be 1;
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);
                } else {
                    //
                }
                if ((EnabledLinkID & 0x8U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr3;
                    Data = ((pSerdesConfig->DataType[3]) | 0x40U);  //bit[6] should be 1;
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);
                } else {
                    //
                }
                SerAddr = MX01_IDC_ADDR_MAX9295_BC;

            }

            if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                if (pSerdesConfig->DataType2[0] != 0xffU) {
                    // Support to receive secondary datatype
                    if (BCFlag[2U] == 0U) {
                        Data = ((pSerdesConfig->DataType2[0]) | 0x40U);  //bit[6] should be 1;
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);
                    } else {
                        if ((EnabledLinkID & 0x1U) != 0U) {
                            SerAddr = pCurSerDesInfo->SerAddr0;
                            Data = ((pSerdesConfig->DataType2[0]) | 0x40U);  //bit[6] should be 1;
                            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);
                        } else {
                            //
                        }
                        if ((EnabledLinkID & 0x2U) != 0U) {
                            SerAddr = pCurSerDesInfo->SerAddr1;
                            Data = ((pSerdesConfig->DataType2[1]) | 0x40U);  //bit[6] should be 1;
                            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);
                        } else {
                            //
                        }
                        if ((EnabledLinkID & 0x4U) != 0U) {
                            SerAddr = pCurSerDesInfo->SerAddr2;
                            Data = ((pSerdesConfig->DataType2[2]) | 0x40U);  //bit[6] should be 1;
                            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);
                        } else {
                            //
                        }
                        if ((EnabledLinkID & 0x8U) != 0U) {
                            SerAddr = pCurSerDesInfo->SerAddr3;
                            Data = ((pSerdesConfig->DataType2[3]) | 0x40U);  //bit[6] should be 1;
                            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);
                        } else {
                            //
                        }
                        SerAddr = MX01_IDC_ADDR_MAX9295_BC;

                    }

                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                } else {
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                }
            } else {
                //FAE's suggestion
                if ((pSerdesConfig->DataType2[0] != 0xffU) && (EnabledLinkID == 0x1U)) {
                    SerAddr = pCurSerDesInfo->SerAddr0;
                    // Support to receive secondary datatype for LinkA only
                    Data = ((pSerdesConfig->DataType2[0]) | 0x40U);  //bit[6] should be 1;
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);

                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                    SerAddr = MX01_IDC_ADDR_MAX9295_BC;
                } else {
                    // not support to receive secondary datatype for multi-chan
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                }
            }
#else
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Normal Serializer", 0U, 0U, 0U, 0U, 0U);
            //LinkA
            if ((EnabledLinkID & 0x1U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr0;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[0];
                SensorAliasID = pSerdesConfig->SensorAliasID[0];

                // --------------------------------------------------------------------------------------
                // MAX9295A powers up in GMSL2 mode
                // Config MIPI PHY / Controller
                // Set SER is in 1x4 mode; 4-lane in controller 1 for port A; Default lane mapping
                //----------------------------------------------------------------------------------------
                //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************         Link A: Serializer MAX9295A         *********************", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[0] - 1U) << 4U)); // select number of data lanes
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U);

                Data = ((pSerdesConfig->DataType[0]) | 0x40U);  //bit[6] should be 1;
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);

                if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                    if (pSerdesConfig->DataType2[0] != 0xffU) {
                        // Support to receive secondary datatype
                        Data = ((pSerdesConfig->DataType2[0]) | 0x40U);  //bit[6] should be 1;
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);

                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                    } else {
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                    }
                } else {
                    //FAE's suggestion
                    if ((pSerdesConfig->DataType2[0] != 0xffU) && (EnabledLinkID == 0x1U)) {
                        // Support to receive secondary datatype for LinkA only
                        Data = ((pSerdesConfig->DataType2[0]) | 0x40U);  //bit[6] should be 1;
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);

                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                    } else {
                        // not support to receive secondary datatype for multi-chan
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                    }
                }
                // -----------------------------------------------------------
                //  sensor I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);

            } else {
                //not LinkA
            }
            //LinkB
            if ((EnabledLinkID & 0x2U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr1;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[1];
                SensorAliasID = pSerdesConfig->SensorAliasID[1];

                // --------------------------------------------------------------------------------------
                // MAX9295A powers up in GMSL2 mode
                // Config MIPI PHY / Controller
                // Set SER is in 1x4 mode; 4-lane in controller 1 for port B; Default lane mapping
                //----------------------------------------------------------------------------------------
                //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************         Link B: Serializer MAX9295A         *********************", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[1] - 1U) << 4U)); // select number of data lanes for Port B.
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U);

                Data = ((pSerdesConfig->DataType[1]) | 0x40U);  //bit[6] should be 1;
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);

                if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                    if (pSerdesConfig->DataType2[1] != 0xffU) {
                        // Support to receive secondary datatype
                        Data = ((pSerdesConfig->DataType2[1]) | 0x40U);  //bit[6] should be 1;
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);

                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                    } else {
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                    }
                } else {
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                }

                // -----------------------------------------------------------
                //  sensor I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);


            } else {
                //not LinkB
            }
            //LinkC
            if ((EnabledLinkID & 0x4U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr2;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[2];
                SensorAliasID = pSerdesConfig->SensorAliasID[2];

                // --------------------------------------------------------------------------------------
                // MAX9295A powers up in GMSL2 mode
                // Config MIPI PHY / Controller
                // Set SER is in 1x4 mode; 4-lane in controller 1 for port C; Default lane mapping
                //----------------------------------------------------------------------------------------
                //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************         Link C: Serializer MAX9295A         *********************", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[2] - 1U) << 4U)); // select number of data lanes for Port C.
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U);

                Data = ((pSerdesConfig->DataType[2]) | 0x40U);  //bit[6] should be 1;
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);

                if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                    if (pSerdesConfig->DataType2[2] != 0xffU) {
                        // Support to receive secondary datatype
                        Data = ((pSerdesConfig->DataType2[2]) | 0x40U);  //bit[6] should be 1;
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);

                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                    } else {
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                    }
                } else {
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                }

                // -----------------------------------------------------------
                //  sensor I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);
            } else {
                //not LinkC
            }
            //LinkD
            if ((EnabledLinkID & 0x8U) != 0U) {

                SerAddr = pCurSerDesInfo->SerAddr3;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[3];
                SensorAliasID = pSerdesConfig->SensorAliasID[3];

                // --------------------------------------------------------------------------------------
                // MAX9295A powers up in GMSL2 mode
                // Config MIPI PHY / Controller
                // Set SER is in 1x4 mode; 4-lane in controller 1 for port D; Default lane mapping
                //----------------------------------------------------------------------------------------
                //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************         Link D: Serializer MAX9295A         *********************", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[3] - 1U) << 4U)); // select number of data lanes for Port D.
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U);

                Data = ((pSerdesConfig->DataType[3]) | 0x40U);  //bit[6] should be 1;
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);

                if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                    if (pSerdesConfig->DataType2[3] != 0xffU) {
                        // Support to receive secondary datatype
                        Data = ((pSerdesConfig->DataType2[3]) | 0x40U);  //bit[6] should be 1;
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);

                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                    } else {
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                    }
                } else {
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                }

                // -----------------------------------------------------------
                //  sensor I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);

            } else {
                //not LinkD
            }
#endif
            // TBD: need to check general case
            // -----------------------------------------------------------
            // Deserializer MIPI Setup
            // -----------------------------------------------------------
            //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************           Derserialzier MAX96712            *********************", NULL, NULL, NULL, NULL, NULL);
            if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                // pipe X in link A to video pipe 0, pipe X in link B to video pipe 1
                // pipe X in link C to video pipe 2, pipe X in link D to video pipe 3
                // pipe X in link A to video pipe 4, pipe Y in link B to video pipe 5
                // pipe X in link C to video pipe 6, pipe Y in link D to video pipe 7
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F0U, 0x40U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F1U, 0xC8U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F2U, 0x51U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F3U, 0xD9U);

                Data = 0U;
                for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
                    if(pSerdesConfig->DataType2[i] != 0xffU) {
                        Data |= (UINT8)(0x11U << i);
                    } else {
                        Data |= (UINT8)(0x01U << i);
                    }
                }
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F4U, Data);
            } else {
                // pipe X in link B to video pipe 1, pipe X in link A to video pipe 0
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F0U, 0x40U);
                if (EnabledLinkID == 0xBU) { //A/B/D 3ch case
                    // pipe X in link D to video pipe 2, pipe X in link C to video pipe 3
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F1U, 0x8CU);
                    // Enable pipe 0 and pipe 1 and pipe 2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F4U, 0x07U);
                } else if((EnabledLinkID == 0x3U) || (EnabledLinkID == 0x7U) || (EnabledLinkID == 0xFU)) { //AB_2ch/ABC_3ch/ABCD_4ch case
                    // pipe X in link D to video pipe 3, pipe X in link C to video pipe 2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F1U, 0xC8U);
                    // Enable pipe 01(2ch)/0-2(3ch)/0-3(4ch)
                    Data = (UINT8)(EnabledLinkID & 0xFU);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F4U, Data);
                } else if(EnabledLinkID == 0x1U) { //A 1ch case
                    //; MAX9295A pipe Y in link A --> MAX96712 video pipe 4 and use PHY A (bit[3:0] = 0001'b)
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F2U, 0x51U);
                    //; Enable pipe 0 and pipe 4
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F4U, 0x11U);
                } else {
                    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] NG! Incorrect EnabledLinkID %x pipe video assignment", EnabledLinkID, 0U, 0U, 0U, 0U);
                }
            }

            //MIPI PHY Setting
            //Set Des in 2x4 mode
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08A0U, 0x04U);
            //Set Lane Mapping for 4-lane port A
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08A3U, 0xE4U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08A4U, 0xE4U);
            //Set 4 lane D-PHY
#ifndef MX01_CPHY
            //Dphy 4lane
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090AU, 0xC0U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094AU, 0xC0U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098AU, 0xC0U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CAU, 0xC0U);
#else
            //Cphy 2lane
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090AU, 0x60U); //Dphy 4lane 0xC0
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094AU, 0x60U); //Dphy 2lane 0x40
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098AU, 0x60U); //Cphy 2lane 0x60
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CAU, 0x60U); //Cphy 1lane 0x20

            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08ADU, 0x3FU); //enlarge t3-PREBEGIN for CV5 limitations
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08AEU, 0x7CU); //enlarge t3-POST for CV5 limitations
#endif
            //Turn on MIPI PHYs PHY0 and PHY1
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08A2U, 0x30U);
            //Put DPLL in reset while changing MIPI data rate
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x1D00U, 0xF4U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x1E00U, 0xF4U);

            // CSI bit rate per lane, CSITxSpeed * 100 MHz
            Data = 0x20U | pSerdesConfig->CSITxSpeed[0];
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0415U, Data);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0418U, Data);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x041BU, Data);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x041EU, Data);

            //release reset to DPLL1 and DPLL2
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x1D00U, 0xF5U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x1E00U, 0xF5U);

            if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                //LinkA
                if ((EnabledLinkID & 0x1U) != 0U) {
                    //video pipe 0
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090BU, 0x07U); // mapping enable (low byte)
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x092DU, 0x15U); // map to MIPI Controller 1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090DU, pSerdesConfig->DataType[0]); //[src]
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090EU, pSerdesConfig->DataType[0]); // [dst] map data to VC0
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090FU, 0x00U); // [src]
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0910U, 0x00U); // [dst] map FS to VC0
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0911U, 0x01U); // [src]
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0912U, 0x01U); // [dst] map FE to VC0

                    // video pipe 4
                    if(pSerdesConfig->DataType2[0] != 0xffU) {
                        // Secondary datatype
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0BU, 0x01U); // mapping enable (low byte)
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A2DU, 0x01U); // map to MIPI Controller 1
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0DU, pSerdesConfig->DataType2[0]);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0EU, pSerdesConfig->DataType2[0]); // map data to VC0
                    } else {
                        // only one datatype
                    }
                }

                //LinkB
                if ((EnabledLinkID & 0x2U) != 0U) {
                    // video pipe 1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094BU, 0x07U); // mapping enable (low byte)
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x096DU, 0x15U); // map to MIPI Controller 1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094DU, pSerdesConfig->DataType[1]);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094EU, pSerdesConfig->DataType[1] | (0x1U << 6U)); // map data to VC1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094FU, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0950U, 0x40U); // map FS to VC1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0951U, 0x01U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0952U, 0x41U); // map FE to VC1

                    // video pipe 5
                    if(pSerdesConfig->DataType2[1] != 0xffU) {
                        // Secondary datatype
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A4BU, 0x01U); // mapping enable (low byte)
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A6DU, 0x01U); // map to MIPI Controller 1
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A4DU, pSerdesConfig->DataType2[1]);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A4EU, pSerdesConfig->DataType2[1] | (0x1U << 6U)); // map data to VC1
                    } else {
                        // only one datatype
                    }
                }

                //LinkC
                if ((EnabledLinkID & 0x4U) != 0U) {
                    // video pipe 2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098BU, 0x07U); // mapping enable (low byte)
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09ADU, 0x15U); // map to MIPI Controller 1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098DU, pSerdesConfig->DataType[2]);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098EU, pSerdesConfig->DataType[2] | (0x2U << 6U)); // map data to VC2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098FU, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0990U, 0x80U); // map FS to VC2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0991U, 0x01U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0992U, 0x81U); // map FE to VC2

                    // video pipe 6
                    if(pSerdesConfig->DataType2[2] != 0xffU) {
                        // Secondary datatype
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A8BU, 0x01U); // mapping enable (low byte)
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0AADU, 0x01U); // map to MIPI Controller 1
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A8DU, pSerdesConfig->DataType2[2]);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A8EU, pSerdesConfig->DataType2[2] | (0x2U << 6U)); // map data to VC2
                    } else {
                        // only one datatype
                    }
                }

                //LinkD
                if ((EnabledLinkID & 0x8U) != 0U) {
                    // video pipe 3
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CBU, 0x07U); // mapping enable (low byte)
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09EDU, 0x15U); // map to MIPI Controller 1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CDU, pSerdesConfig->DataType[3]);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CEU, pSerdesConfig->DataType[3] | (0x3U << 6U)); // map data to VC3
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CFU, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09D0U, 0xC0U); // map FS to VC3
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09D1U, 0x01U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09D2U, 0xC1U); // map FE to VC3

                    // video pipe 7
                    if (pSerdesConfig->DataType2[3] != 0xffU) {
                        // Secondary datatype
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0ACBU, 0x01U); // mapping enable (low byte)
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0AEDU, 0x01U); // map to MIPI Controller 1
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0ACDU, pSerdesConfig->DataType2[3]);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0ACEU, pSerdesConfig->DataType2[3] | (0x3U << 6U)); // map data to VC3
                    } else {
                        // only one datatype
                    }
                }

                //important !!! Depend on different silicon to disable FS/FE output.
                //ES1. Reg 0x259/0x279/0x299/0x2B9 = 0x39 (bit 5 = 1, force VS = 0) for pipe 4/5/6/7
                //ES2. Reg 0x436 (bit 4/5/6/7 = 1, force VS = 0) for pipe 4/5/6/7
                Data = (UINT8)(EnabledLinkID << 4U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0436U, Data);
            } else {
                if (EnabledLinkID == 1U) {
                    //; Video Pipe to MIPI Controller Mapping
                    //; RAW12, video pipe 0
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090BU, 0x07U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x092DU, 0x15U);

                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090DU, pSerdesConfig->DataType[0]);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090EU, pSerdesConfig->DataType[0]);

                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090FU, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0910U, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0911U, 0x01U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0912U, 0x01U);

                    if(pSerdesConfig->DataType2[0] != 0xffU) {
                        //; Secondary datatype, video pipe 4, map FS/FE
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0BU, 0x01U);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A2DU, 0x01U);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0DU, pSerdesConfig->DataType2[0]);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0EU, pSerdesConfig->DataType2[0]);
                    } else {
                        // 1-ch with only one datatype
                    }

                    //important !!! Depend on different silicon to disable FS/FE output.
                    //ES1. Reg 0x259 = 0x39 (bit 5 = 1, force VS = 0) for pipe 4
                    //ES2. Reg 0x436 = 0x10 (bit 4 = 1, force VS = 0) for pipe 4
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0436U, 0x10U);

                } else {
                    //not 1ch case
                    //; MIPI controller 1 Concatentation register = b'10000001
                    if(EnabledLinkID == 0xBU) {
                        if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_LINE_INTERLEAVED) {
                            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0971U, 0x07U);
                        } else {
                            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0971U, 0x87U);
                        }
                    } else if((EnabledLinkID == 0x3U) || (EnabledLinkID == 0x7U) || (EnabledLinkID == 0xFU)) {
                        if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_LINE_INTERLEAVED) {
                            Data = (UINT8)(EnabledLinkID & 0xFU);
                            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0971U, Data);
                        } else {
                            Data = (UINT8)(EnabledLinkID & 0xFU);
                            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0971U, (Data | 0x80U));
                        }
                    } else {
                        AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] NG! Incorrect EnabledLinkID %d Concatentation register", EnabledLinkID, 0U, 0U, 0U, 0U);
                    }

                }
            }

            //VS debug signal Output on MFP4
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00FAU, 0x10U);

            // -----------------------------------------------------------
            // XHS and XVS GPIO forwarding setup
            // -----------------------------------------------------------
            AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; XVS connects to MAX96712 MFP2?->?MAX9295A MFP7", NULL, NULL, NULL, NULL, NULL);
#ifdef SERIALIZER_BC
            // XVS signal from SoC, DES_MFP2 --> SER_MFP7
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D5U, 0x42U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D3U, 0x84U);
            // XHS signal from SoC, DES_MFP9 --> SER_MFP0
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02C0U, 0x49U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BEU, 0x84U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BFU, 0x20U);

            //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
            //MFP3
            RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02c8, &Data);
            Data = (Data | 0x80U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02c8, Data);
            //MFP5
            RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02ce, &Data);
            Data = (Data | 0x80U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02ce, Data);
            //MFP6
            RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02d1, &Data);
            Data = (Data | 0x80U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02d1, Data);
#else
            //LinkA
            if ((EnabledLinkID & 0x1U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr0;

                // XVS signal from SoC, DES_MFP2 --> SER_MFP7
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D5U, 0x42U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D3U, 0x84U);
                // XHS signal from SoC, DES_MFP9 --> SER_MFP0
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02C0U, 0x49U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BEU, 0x84U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BFU, 0x20U);

                //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
                //MFP3
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02c8, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02c8, Data);
                //MFP5
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02ce, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02ce, Data);
                //MFP6
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02d1, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02d1, Data);

            } else {
                //not LinkA
            }
            //LinkB
            if ((EnabledLinkID & 0x2U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr1;

                // XVS signal from SoC, DES_MFP2 --> SER_MFP7
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D5U, 0x42U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D3U, 0x84U);
                // XHS signal from SoC, DES_MFP9 --> SER_MFP0
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02C0U, 0x49U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BEU, 0x84U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BFU, 0x20U);

                //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
                //MFP3
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02c8, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02c8, Data);
                //MFP5
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02ce, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02ce, Data);
                //MFP6
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02d1, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02d1, Data);
            } else {
                //not LinkB
            }
            //LinkC
            if ((EnabledLinkID & 0x4U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr2;

                // XVS signal from SoC, DES_MFP2 --> SER_MFP7
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D5U, 0x42U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D3U, 0x84U);
                // XHS signal from SoC, DES_MFP9 --> SER_MFP0
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02C0U, 0x49U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BEU, 0x84U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BFU, 0x20U);

                //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
                //MFP3
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02c8, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02c8, Data);
                //MFP5
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02ce, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02ce, Data);
                //MFP6
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02d1, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02d1, Data);
            } else {
                //not LinkC
            }
            //LinkD
            if ((EnabledLinkID & 0x8U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr3;

                // XVS signal from SoC, DES_MFP2 --> SER_MFP7
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D5U, 0x42U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D3U, 0x84U);
                // XHS signal from SoC, DES_MFP9 --> SER_MFP0
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02C0U, 0x49U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BEU, 0x84U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BFU, 0x20U);

                //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
                //MFP3
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02c8, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02c8, Data);
                //MFP5
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02ce, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02ce, Data);
                //MFP6
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02d1, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02d1, Data);
            } else {
                //not LinkD
            }
#endif
            AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; Set Internal FSYNC off, GPIO is used for FSYNC, type = GMSL2", NULL, NULL, NULL, NULL, NULL);
            // Set Internal FSYNC off, GPIO is used for FSYNC, type = GMSL2
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x04A0U, 0x08U); //FSYNC_MODE = Frame sync generation is off, GPIO is used for FSYNC, type = GMSL2
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x04AFU, 0x9FU);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0306U, 0x83U); //GPIO_A: MAX96712 MFP2 TX_EN = 1, OUT_DIS = 1, COMP_EN = 0
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x031CU, 0x83U); //GPIO_A: MAX96712 MFP9 TX_EN = 1, OUT_DIS = 1, COMP_EN = 0
            // Config MAX96712 MFP2 to receive external FSYNC signal for each link
            //LinkA
            if (EnabledLinkID == 0x1U) {
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x03AAU, 0x22U); //[TBD] set for 1ch
            } else {
                //not LinkA
            }
            //LinkB
            if ((EnabledLinkID & 0x2U) != 0U) {
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x033DU, 0x22U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0354U, 0x29U); //[TBD] r0x0374U=0x22: 1ch/IMX390
            } else {
                //not LinkB
            }
            //LinkC
            if ((EnabledLinkID & 0x4U) != 0U) {
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0374U, 0x22U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x038AU, 0x29U);
            } else {
                //not LinkC
            }
            //LinkD
            if ((EnabledLinkID & 0x8U) != 0U) {
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x03AAU, 0x22U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x03C1U, 0x29U);
            } else {
                //not LinkD
            }
#ifdef SERIALIZER_BC
            //sensor I2C tanslation for broadcast case
            // -----------------------------------------------------------
            //  sensor I2C tanslation
            // -----------------------------------------------------------
            //LinkA
            if ((EnabledLinkID & 0x1U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr0;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[0];
                SensorAliasID = pSerdesConfig->SensorAliasID[0];
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);
            } else {
                //
            }
            //LinkB
            if ((EnabledLinkID & 0x2U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr1;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[1];
                SensorAliasID = pSerdesConfig->SensorAliasID[1];
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);
            } else {
                //
            }
            //LinkC
            if ((EnabledLinkID & 0x4U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr2;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[2];
                SensorAliasID = pSerdesConfig->SensorAliasID[2];
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);
            } else {
                //
            }
            //LinkD
            if ((EnabledLinkID & 0x8U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr3;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[3];
                SensorAliasID = pSerdesConfig->SensorAliasID[3];
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);
            } else {
                //
            }
#endif

        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_Config2
 *
 *  @Description:: Configure Max9295_96712
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
UINT32 Max9295_96712_VCXC_Config2(UINT32 VinID, const MAX9295_96712_SERDES_CONFIG2_s *pSerdesConfig)
{
    const MAX9295_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    UINT16 EnabledLinkID;
    UINT8 SensorSlaveID, SensorAliasID, SensorBCID;
    UINT8 SerAddr, DesAddr;
    UINT8 Data;
    UINT32 i;
#ifdef SERIALIZER_BC
    UINT8 BCInit = 0; //1: record 1st Config Chan value
    UINT8 BCCSIRxLaneNum = 0U;
    UINT8 BCDataType = 0U;
    UINT8 BCDataType2 = 0U;
    UINT16 BCFlag[3]; //BCFlag[]=0 when CSIRxLaneNum/DataType/DataType2 is the same for each Chan

    //init BCFlag
    for (i=0; i < 3U; i++) {
        BCFlag[i] = 0U;
    }
#endif

    if (pSerdesConfig == NULL) {
        RetVal = MAX9295_96712_ERR_ARG;
    } else {
        if (pCurSerDesInfo->InitDone == 0U) {
            RetVal = MAX9295_96712_ERR_INVALID_API;
        } else {
            EnabledLinkID = pSerdesConfig->EnabledLinkID;
            DesAddr = pCurSerDesInfo->DesAddr;
            SensorBCID = pSerdesConfig->SensorBCID;

#ifndef MX01_CPHY
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] MIPI DPHY", 0U, 0U, 0U, 0U, 0U);
#else
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] MIPI CPHY", 0U, 0U, 0U, 0U, 0U);
#endif
            if (pSerdesConfig->VideoAggregationMode != MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                if((EnabledLinkID == 0x3U) || (EnabledLinkID == 0x7U) || (EnabledLinkID == 0xbU) || (EnabledLinkID == 0xfU)) {
                    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Config (EnabledLinkID: 0x%02x)... transfer RAW12 only(multi-ch)", EnabledLinkID, 0U, 0U, 0U, 0U);
                } else if(EnabledLinkID == 0x1U) {
                    if(pSerdesConfig->DataType2[0] != 0xffU) {
                        AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Config (EnabledLinkID: 0x%02x)... transfer RAW%d+OB (1-ch)", EnabledLinkID, pSerdesConfig->NumDataBits[0], 0U, 0U, 0U);
                    } else {
                        AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Config (EnabledLinkID: 0x%02x)... transfer RAW%d only(1-ch)", EnabledLinkID, pSerdesConfig->NumDataBits[0], 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] NG! Incorrect SensorID 0x%02x", EnabledLinkID, 0U, 0U, 0U, 0U);
                    RetVal = MAX9295_96712_ERR_ARG;
                }
            }
#ifdef REDUCE_BOOT_TIME
            if (EnabledLinkID == 1U) {
#endif
                AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; GMSL2 mode for all links and only Link EnabledLinkID", NULL, NULL, NULL, NULL, NULL);
                //GMSL2 mode for all links and only Link by EnabledLinkID
                Data = (UINT8)(0xF0U + (EnabledLinkID & 0xFU));
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, Data);
                //Set 6Gbps link Rx rate for GMSL2
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0010U, 0x22U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0011U, 0x22U);
                //One-shot link reset for EnabledLinkID
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0018U, 0x0FU);
                (void)AmbaKAL_TaskSleep(100U);
                //Check locked
                (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
                RetVal |= Max9295_96712_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200, EnabledLinkID);
#ifdef REDUCE_BOOT_TIME
            } else {
                //
            }
#endif

            //I2C slave address translation

            AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; I2C slave address translation", NULL, NULL, NULL, NULL, NULL);
            //LinkA
            if ((EnabledLinkID & 0x1U) != 0U) {
                //Enable Link A Only and change Link A MAX9295A slave address to 0x82
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, 0xF1U);
#ifndef REDUCE_BOOT_TIME
                (void)AmbaKAL_TaskSleep(100U);
#else
                (void)AmbaKAL_TaskSleep(45U);
#endif
                RetVal |= Max9295_96712_RegWrite(VinID, MX01_IDC_ADDR_MAX9295_DEFAULT, 0x0000U, pCurSerDesInfo->SerAddr0);

            }
            //LinkB
            if ((EnabledLinkID & 0x2U) != 0U) {
                //Enable Link B Only and change Link B MAX9295A slave address to 0x84
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, 0xF2U);
#ifndef REDUCE_BOOT_TIME
                (void)AmbaKAL_TaskSleep(100U);
#else
                (void)AmbaKAL_TaskSleep(45U);
#endif
                RetVal |= Max9295_96712_RegWrite(VinID, MX01_IDC_ADDR_MAX9295_DEFAULT, 0x0000U, pCurSerDesInfo->SerAddr1);

            }
            //LinkC
            if ((EnabledLinkID & 0x4U) != 0U) {
                //TBD: FAE doesn't provide setting yet
                //Enable Link C Only and change Link C MAX9295A slave address to 0x86
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, 0xF4U);
#ifndef REDUCE_BOOT_TIME
                (void)AmbaKAL_TaskSleep(100U);
#else
                (void)AmbaKAL_TaskSleep(45U);
#endif
                RetVal |= Max9295_96712_RegWrite(VinID, MX01_IDC_ADDR_MAX9295_DEFAULT, 0x0000U, pCurSerDesInfo->SerAddr2);
            }
            //LinkD
            if ((EnabledLinkID & 0x8U) != 0U) {
                //Enable Link C Only and change Link C MAX9295A slave address to 0x88
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, 0xF8U);
#ifndef REDUCE_BOOT_TIME
                (void)AmbaKAL_TaskSleep(100U);
#else
                (void)AmbaKAL_TaskSleep(45U);
#endif
                RetVal |= Max9295_96712_RegWrite(VinID, MX01_IDC_ADDR_MAX9295_DEFAULT, 0x0000U, pCurSerDesInfo->SerAddr3);
            }
            if ((EnabledLinkID & 0xeU) != 0U) {
                AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; Re-enable multi-ch", NULL, NULL, NULL, NULL, NULL);
                Data = (UINT8)(0xF0U + (EnabledLinkID & 0xFU));
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, Data);
#ifndef REDUCE_BOOT_TIME
                (void)AmbaKAL_TaskSleep(100U);
#else
                (void)AmbaKAL_TaskSleep(45U);
#endif
            }

#ifdef SERIALIZER_BC
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] SERIALIZER_BC", 0U, 0U, 0U, 0U, 0U);
            //Serializer BC slave I2C
            if ((EnabledLinkID & 0x1U) != 0U) {
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x0042U, MX01_IDC_ADDR_MAX9295_BC);
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x0043U, pCurSerDesInfo->SerAddr0);
            } else {
                //
            }
            if ((EnabledLinkID & 0x2U) != 0U) {
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x0042U, MX01_IDC_ADDR_MAX9295_BC);
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x0043U, pCurSerDesInfo->SerAddr1);
            } else {
                //
            }
            if ((EnabledLinkID & 0x4U) != 0U) {
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr2, 0x0042U, MX01_IDC_ADDR_MAX9295_BC);
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr2, 0x0043U, pCurSerDesInfo->SerAddr2);
            } else {
                //
            }
            if ((EnabledLinkID & 0x8U) != 0U) {
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr3, 0x0042U, MX01_IDC_ADDR_MAX9295_BC);
                (void)Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr3, 0x0043U, pCurSerDesInfo->SerAddr3);
            } else {
                //
            }
            SerAddr = MX01_IDC_ADDR_MAX9295_BC;

            for (i=0U; i < MAX96712_NUM_RX_PORT; i ++) {

                if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
                    if(BCInit == 0U) {
                        BCCSIRxLaneNum = pSerdesConfig->CSIRxLaneNum[i];
                        BCDataType = pSerdesConfig->DataType[i];
                        BCDataType2 = pSerdesConfig->CSIRxLaneNum[i];
                        BCInit = 1U;
                    } else {
                        if (BCFlag[0] == 0U) {
                            if (pSerdesConfig->CSIRxLaneNum[i] != BCCSIRxLaneNum) {
                                BCFlag[0] = 1U;
                            } else {
                                //
                            }
                        } else {
                            //
                        }
                        if (BCFlag[1] == 0U) {
                            if (pSerdesConfig->DataType[i] != BCDataType) {
                                BCFlag[1] = 1U;
                            } else {
                                //
                            }
                        } else {
                            //
                        }
                        if (BCFlag[2] == 0U)  {
                            if(pSerdesConfig->DataType2[i] != BCDataType2) {
                                BCFlag[2] = 1U;
                            } else {
                                //
                            }
                        } else {
                            //
                        }
                    }

                }
            }

            // --------------------------------------------------------------------------------------
            // MAX9295A powers up in GMSL2 mode
            // Config MIPI PHY / Controller
            // Set SER is in 1x4 mode; 4-lane in controller 1 for port A; Default lane mapping
            //----------------------------------------------------------------------------------------
            //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************         Link A: Serializer MAX9295A         *********************", NULL, NULL, NULL, NULL, NULL);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
            if(BCFlag[0] == 0U) {
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[0] - 1U) << 4U)); // select number of data lanes
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
            } else {
                if ((EnabledLinkID & 0x1U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr0;
                    Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[0] - 1U) << 4U)); // select number of data lanes
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                } else {
                    //
                }
                if ((EnabledLinkID & 0x2U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr1;
                    Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[1] - 1U) << 4U)); // select number of data lanes
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                } else {
                    //
                }
                if ((EnabledLinkID & 0x4U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr2;
                    Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[2] - 1U) << 4U)); // select number of data lanes
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                } else {
                    //
                }
                if ((EnabledLinkID & 0x8U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr3;
                    Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[3] - 1U) << 4U)); // select number of data lanes
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                } else {
                    //
                }
                SerAddr = MX01_IDC_ADDR_MAX9295_BC;
            }
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U);

            if(BCFlag[1U] == 0U) {
                Data = ((pSerdesConfig->DataType[0]) | 0x40U);  //bit[6] should be 1;
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);
            } else {
                if ((EnabledLinkID & 0x1U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr0;
                    Data = ((pSerdesConfig->DataType[0]) | 0x40U);  //bit[6] should be 1;
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);
                } else {
                    //
                }
                if ((EnabledLinkID & 0x2U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr1;
                    Data = ((pSerdesConfig->DataType[1]) | 0x40U);  //bit[6] should be 1;
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);
                } else {
                    //
                }
                if ((EnabledLinkID & 0x4U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr2;
                    Data = ((pSerdesConfig->DataType[2]) | 0x40U);  //bit[6] should be 1;
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);
                } else {
                    //
                }
                if ((EnabledLinkID & 0x8U) != 0U) {
                    SerAddr = pCurSerDesInfo->SerAddr3;
                    Data = ((pSerdesConfig->DataType[3]) | 0x40U);  //bit[6] should be 1;
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);
                } else {
                    //
                }
                SerAddr = MX01_IDC_ADDR_MAX9295_BC;

            }

            if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                if (pSerdesConfig->DataType2[0] != 0xffU) {
                    // Support to receive secondary datatype
                    if (BCFlag[2U] == 0U) {
                        Data = ((pSerdesConfig->DataType2[0]) | 0x40U);  //bit[6] should be 1;
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);
                    } else {
                        if ((EnabledLinkID & 0x1U) != 0U) {
                            SerAddr = pCurSerDesInfo->SerAddr0;
                            Data = ((pSerdesConfig->DataType2[0]) | 0x40U);  //bit[6] should be 1;
                            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);
                        } else {
                            //
                        }
                        if ((EnabledLinkID & 0x2U) != 0U) {
                            SerAddr = pCurSerDesInfo->SerAddr1;
                            Data = ((pSerdesConfig->DataType2[1]) | 0x40U);  //bit[6] should be 1;
                            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);
                        } else {
                            //
                        }
                        if ((EnabledLinkID & 0x4U) != 0U) {
                            SerAddr = pCurSerDesInfo->SerAddr2;
                            Data = ((pSerdesConfig->DataType2[2]) | 0x40U);  //bit[6] should be 1;
                            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);
                        } else {
                            //
                        }
                        if ((EnabledLinkID & 0x8U) != 0U) {
                            SerAddr = pCurSerDesInfo->SerAddr3;
                            Data = ((pSerdesConfig->DataType2[3]) | 0x40U);  //bit[6] should be 1;
                            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);
                        } else {
                            //
                        }
                        SerAddr = MX01_IDC_ADDR_MAX9295_BC;

                    }

                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                } else {
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                }
            } else {
                //FAE's suggestion
                if ((pSerdesConfig->DataType2[0] != 0xffU) && (EnabledLinkID == 0x1U)) {
                    SerAddr = pCurSerDesInfo->SerAddr0;
                    // Support to receive secondary datatype for LinkA only
                    Data = ((pSerdesConfig->DataType2[0]) | 0x40U);  //bit[6] should be 1;
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);

                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                    SerAddr = MX01_IDC_ADDR_MAX9295_BC;
                } else {
                    // not support to receive secondary datatype for multi-chan
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                }
            }
#else
            AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Normal Serializer", 0U, 0U, 0U, 0U, 0U);
            //LinkA
            if ((EnabledLinkID & 0x1U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr0;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[0];
                SensorAliasID = pSerdesConfig->SensorAliasID[0];

                // --------------------------------------------------------------------------------------
                // MAX9295A powers up in GMSL2 mode
                // Config MIPI PHY / Controller
                // Set SER is in 1x4 mode; 4-lane in controller 1 for port A; Default lane mapping
                //----------------------------------------------------------------------------------------
                //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************         Link A: Serializer MAX9295A         *********************", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[0] - 1U) << 4U)); // select number of data lanes
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U);

                Data = ((pSerdesConfig->DataType[0]) | 0x40U);  //bit[6] should be 1;
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);

                if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                    if (pSerdesConfig->DataType2[0] != 0xffU) {
                        // Support to receive secondary datatype
                        Data = ((pSerdesConfig->DataType2[0]) | 0x40U);  //bit[6] should be 1;
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);

                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                    } else {
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                    }
                } else {
                    //FAE's suggestion
                    if ((pSerdesConfig->DataType2[0] != 0xffU) && (EnabledLinkID == 0x1U)) {
                        // Support to receive secondary datatype for LinkA only
                        Data = ((pSerdesConfig->DataType2[0]) | 0x40U);  //bit[6] should be 1;
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);

                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                    } else {
                        // not support to receive secondary datatype for multi-chan
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                    }
                }
                // -----------------------------------------------------------
                //  sensor I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);

            } else {
                //not LinkA
            }
            //LinkB
            if ((EnabledLinkID & 0x2U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr1;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[1];
                SensorAliasID = pSerdesConfig->SensorAliasID[1];

                // --------------------------------------------------------------------------------------
                // MAX9295A powers up in GMSL2 mode
                // Config MIPI PHY / Controller
                // Set SER is in 1x4 mode; 4-lane in controller 1 for port B; Default lane mapping
                //----------------------------------------------------------------------------------------
                //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************         Link B: Serializer MAX9295A         *********************", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[1] - 1U) << 4U)); // select number of data lanes for Port B.
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U);

                Data = ((pSerdesConfig->DataType[1]) | 0x40U);  //bit[6] should be 1;
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);

                if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                    if (pSerdesConfig->DataType2[1] != 0xffU) {
                        // Support to receive secondary datatype
                        Data = ((pSerdesConfig->DataType2[1]) | 0x40U);  //bit[6] should be 1;
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);

                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                    } else {
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                    }
                } else {
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                }

                // -----------------------------------------------------------
                //  sensor I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);


            } else {
                //not LinkB
            }
            //LinkC
            if ((EnabledLinkID & 0x4U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr2;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[2];
                SensorAliasID = pSerdesConfig->SensorAliasID[2];

                // --------------------------------------------------------------------------------------
                // MAX9295A powers up in GMSL2 mode
                // Config MIPI PHY / Controller
                // Set SER is in 1x4 mode; 4-lane in controller 1 for port C; Default lane mapping
                //----------------------------------------------------------------------------------------
                //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************         Link C: Serializer MAX9295A         *********************", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[2] - 1U) << 4U)); // select number of data lanes for Port C.
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U);

                Data = ((pSerdesConfig->DataType[2]) | 0x40U);  //bit[6] should be 1;
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);

                if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                    if (pSerdesConfig->DataType2[2] != 0xffU) {
                        // Support to receive secondary datatype
                        Data = ((pSerdesConfig->DataType2[2]) | 0x40U);  //bit[6] should be 1;
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);

                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                    } else {
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                    }
                } else {
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                }

                // -----------------------------------------------------------
                //  sensor I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);
            } else {
                //not LinkC
            }
            //LinkD
            if ((EnabledLinkID & 0x8U) != 0U) {

                SerAddr = pCurSerDesInfo->SerAddr3;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[3];
                SensorAliasID = pSerdesConfig->SensorAliasID[3];

                // --------------------------------------------------------------------------------------
                // MAX9295A powers up in GMSL2 mode
                // Config MIPI PHY / Controller
                // Set SER is in 1x4 mode; 4-lane in controller 1 for port D; Default lane mapping
                //----------------------------------------------------------------------------------------
                //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************         Link D: Serializer MAX9295A         *********************", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum[3] - 1U) << 4U)); // select number of data lanes for Port D.
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U);

                Data = ((pSerdesConfig->DataType[3]) | 0x40U);  //bit[6] should be 1;
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);

                if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                    if (pSerdesConfig->DataType2[3] != 0xffU) {
                        // Support to receive secondary datatype
                        Data = ((pSerdesConfig->DataType2[3]) | 0x40U);  //bit[6] should be 1;
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);

                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
                    } else {
                        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                    }
                } else {
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
                }

                // -----------------------------------------------------------
                //  sensor I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);

            } else {
                //not LinkD
            }
#endif
            // TBD: need to check general case
            // -----------------------------------------------------------
            // Deserializer MIPI Setup
            // -----------------------------------------------------------
            //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************           Derserialzier MAX96712            *********************", NULL, NULL, NULL, NULL, NULL);
            if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                // pipe X in link A to video pipe 0, pipe X in link B to video pipe 1
                // pipe X in link C to video pipe 2, pipe X in link D to video pipe 3
                // pipe X in link A to video pipe 4, pipe Y in link B to video pipe 5
                // pipe X in link C to video pipe 6, pipe Y in link D to video pipe 7
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F0U, 0x40U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F1U, 0xC8U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F2U, 0x51U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F3U, 0xD9U);

                Data = 0U;
                for (i = 0U; i < MAX96712_NUM_RX_PORT; i++) {
                    if(pSerdesConfig->DataType2[i] != 0xffU) {
                        Data |= (UINT8)(0x11U << i);
                    } else {
                        Data |= (UINT8)(0x01U << i);
                    }
                }
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F4U, Data);
            } else {
                // pipe X in link B to video pipe 1, pipe X in link A to video pipe 0
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F0U, 0x40U);
                if (EnabledLinkID == 0xBU) { //A/B/D 3ch case
                    // pipe X in link D to video pipe 2, pipe X in link C to video pipe 3
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F1U, 0x8CU);
                    // Enable pipe 0 and pipe 1 and pipe 2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F4U, 0x07U);
                } else if((EnabledLinkID == 0x3U) || (EnabledLinkID == 0x7U) || (EnabledLinkID == 0xFU)) { //AB_2ch/ABC_3ch/ABCD_4ch case
                    // pipe X in link D to video pipe 3, pipe X in link C to video pipe 2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F1U, 0xC8U);
                    // Enable pipe 01(2ch)/0-2(3ch)/0-3(4ch)
                    Data = (UINT8)(EnabledLinkID & 0xFU);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F4U, Data);
                } else if(EnabledLinkID == 0x1U) { //A 1ch case
                    //; MAX9295A pipe Y in link A --> MAX96712 video pipe 4 and use PHY A (bit[3:0] = 0001'b)
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F2U, 0x51U);
                    //; Enable pipe 0 and pipe 4
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F4U, 0x11U);
                } else {
                    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] NG! Incorrect EnabledLinkID %x pipe video assignment", EnabledLinkID, 0U, 0U, 0U, 0U);
                }
            }

            //MIPI PHY Setting
            //Set Des in 2x4 mode
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08A0U, 0x04U);
            //Set Lane Mapping for 4-lane port A
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08A3U, 0xE4U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08A4U, 0xE4U);
            //Set 4 lane D-PHY
#ifndef MX01_CPHY
            //Dphy 4lane
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090AU, 0xC0U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094AU, 0xC0U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098AU, 0xC0U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CAU, 0xC0U);
#elif defined VCX
            //Cphy 2lane VCX
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090AU, 0x70U); ////bit4, csi2_vcx_en=1 to support VCX
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094AU, 0x70U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098AU, 0x70U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CAU, 0x70U);

            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08ADU, 0x3FU); //enlarge t3-PREBEGIN for CV5 limitations
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08AEU, 0x7CU); //enlarge t3-POST for CV5 limitations
#else
            //Cphy 2lane
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090AU, 0x60U); //Dphy 4lane 0xC0
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094AU, 0x60U); //Dphy 2lane 0x40
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098AU, 0x60U); //Cphy 2lane 0x60
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CAU, 0x60U); //Cphy 1lane 0x20

            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08ADU, 0x3FU); //enlarge t3-PREBEGIN for CV5 limitations
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08AEU, 0x7CU); //enlarge t3-POST for CV5 limitations
#endif
            //Turn on MIPI PHYs PHY0 and PHY1
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08A2U, 0x30U);
            //Put DPLL in reset while changing MIPI data rate
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x1D00U, 0xF4U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x1E00U, 0xF4U);

            // CSI bit rate per lane, CSITxSpeed * 100 MHz
            Data = 0x20U | pSerdesConfig->CSITxSpeed[0];
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0415U, Data);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0418U, Data);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x041BU, Data);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x041EU, Data);

            //release reset to DPLL1 and DPLL2
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x1D00U, 0xF5U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x1E00U, 0xF5U);

            if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_FCFS) {
                //LinkA
                if ((EnabledLinkID & 0x1U) != 0U) {
                    //video pipe 0
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090BU, 0x07U); // mapping enable (low byte)
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x092DU, 0x15U); // map to MIPI Controller 1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090DU, pSerdesConfig->DataType[0]); //[src]
#ifndef VCX
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090EU, pSerdesConfig->DataType[0]); // [dst] map data to VC0
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090FU, 0x00U); // [src]
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0910U, 0x00U); // [dst] map FS to VC0
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0911U, 0x01U); // [src]
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0912U, 0x01U); // [dst] map FE to VC0
#else
                    //VC1 + VCX_0x10 => 0x11
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090EU, pSerdesConfig->DataType[0] | (0x1U << 6U)); // [dst] map data to VC1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0800U, 0x10U); // [src_dst] map data to VCX_0x10
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090FU, 0x00U); // [src]
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0910U, 0x40U); // [dst] map FS to VC1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0801U, 0x10U); // [src_dst] map FS to VCX_0x10
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0911U, 0x01U); // [src]
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0912U, 0x41U); // [dst] map FE to VC1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0802U, 0x10U); // [src_dst] map FE to VCX_0x10
#endif
                    // video pipe 4
                    if(pSerdesConfig->DataType2[0] != 0xffU) {
                        // Secondary datatype
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0BU, 0x01U); // mapping enable (low byte)
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A2DU, 0x01U); // map to MIPI Controller 1
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0DU, pSerdesConfig->DataType2[0]);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0EU, pSerdesConfig->DataType2[0]); // map data to VC0
                    } else {
                        // only one datatype
                    }
                }

                //LinkB
                if ((EnabledLinkID & 0x2U) != 0U) {
                    // video pipe 1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094BU, 0x07U); // mapping enable (low byte)
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x096DU, 0x15U); // map to MIPI Controller 1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094DU, pSerdesConfig->DataType[1]);

#ifndef VCX
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094EU, pSerdesConfig->DataType[1] | (0x1U << 6U)); // map data to VC1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094FU, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0950U, 0x40U); // map FS to VC1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0951U, 0x01U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0952U, 0x41U); // map FE to VC1
#else
                    //VC3 + VCX_0x14 => 0x17
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094EU, pSerdesConfig->DataType[1] | (0x3U << 6U)); // map data to VC3
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0810U, 0x14U); // map data to VCX_0x14
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094FU, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0950U, 0xC0U); // map FS to VC3
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0811U, 0x14U); // map data to VCX_0x14
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0951U, 0x01U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0952U, 0xC1U); // map FE to VC3
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0812U, 0x14U); // map data to VCX_0x14
#endif
                    // video pipe 5
                    if(pSerdesConfig->DataType2[1] != 0xffU) {
                        // Secondary datatype
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A4BU, 0x01U); // mapping enable (low byte)
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A6DU, 0x01U); // map to MIPI Controller 1
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A4DU, pSerdesConfig->DataType2[1]);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A4EU, pSerdesConfig->DataType2[1] | (0x1U << 6U)); // map data to VC1
                    } else {
                        // only one datatype
                    }
                }

                //LinkC
                if ((EnabledLinkID & 0x4U) != 0U) {
                    // video pipe 2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098BU, 0x07U); // mapping enable (low byte)
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09ADU, 0x15U); // map to MIPI Controller 1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098DU, pSerdesConfig->DataType[2]);

#ifndef VCX
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098EU, pSerdesConfig->DataType[2] | (0x2U << 6U)); // map data to VC2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098FU, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0990U, 0x80U); // map FS to VC2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0991U, 0x01U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0992U, 0x81U); // map FE to VC2
#else
                    //VC2 + VCX_C => 0xE
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098EU, pSerdesConfig->DataType[2] | (0x2U << 6U)); // map data to VC2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0820U, 0x0CU); // map data to VCX_C
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098FU, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0990U, 0x80U); // map FS to VC2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0821U, 0x0CU); // map data to VCX_C
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0991U, 0x01U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0992U, 0x81U); // map FE to VC2
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0822U, 0x0CU); // map data to VCX_C
#endif
                    // video pipe 6
                    if(pSerdesConfig->DataType2[2] != 0xffU) {
                        // Secondary datatype
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A8BU, 0x01U); // mapping enable (low byte)
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0AADU, 0x01U); // map to MIPI Controller 1
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A8DU, pSerdesConfig->DataType2[2]);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A8EU, pSerdesConfig->DataType2[2] | (0x2U << 6U)); // map data to VC2
                    } else {
                        // only one datatype
                    }
                }

                //LinkD
                if ((EnabledLinkID & 0x8U) != 0U) {
                    // video pipe 3
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CBU, 0x07U); // mapping enable (low byte)
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09EDU, 0x15U); // map to MIPI Controller 1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CDU, pSerdesConfig->DataType[3]);

#ifndef VCX
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CEU, pSerdesConfig->DataType[3] | (0x3U << 6U)); // map data to VC3
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CFU, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09D0U, 0xC0U); // map FS to VC3
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09D1U, 0x01U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09D2U, 0xC1U); // map FE to VC3
#else
                    //VC1 + VCX_0x1C => 0x1D
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CEU, pSerdesConfig->DataType[3] | (0x1U << 6U)); // map data to VC1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0830U, 0x1CU); // map data to VCX_0x1C
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CFU, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09D0U, 0x40U); // map FS to VC1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0831U, 0x1CU); // map data to VCX_0x1C
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09D1U, 0x01U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09D2U, 0x41U); // map FE to VC1
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0832U, 0x1CU); // map data to VCX_0x1C
#endif
                    // video pipe 7
                    if (pSerdesConfig->DataType2[3] != 0xffU) {
                        // Secondary datatype
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0ACBU, 0x01U); // mapping enable (low byte)
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0AEDU, 0x01U); // map to MIPI Controller 1
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0ACDU, pSerdesConfig->DataType2[3]);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0ACEU, pSerdesConfig->DataType2[3] | (0x3U << 6U)); // map data to VC3
                    } else {
                        // only one datatype
                    }
                }

                //important !!! Depend on different silicon to disable FS/FE output.
                //ES1. Reg 0x259/0x279/0x299/0x2B9 = 0x39 (bit 5 = 1, force VS = 0) for pipe 4/5/6/7
                //ES2. Reg 0x436 (bit 4/5/6/7 = 1, force VS = 0) for pipe 4/5/6/7
                Data = (UINT8)(EnabledLinkID << 4U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0436U, Data);
            } else {
                if (EnabledLinkID == 1U) {
                    //; Video Pipe to MIPI Controller Mapping
                    //; RAW12, video pipe 0
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090BU, 0x07U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x092DU, 0x15U);

                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090DU, pSerdesConfig->DataType[0]);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090EU, pSerdesConfig->DataType[0]);

                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090FU, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0910U, 0x00U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0911U, 0x01U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0912U, 0x01U);

                    if(pSerdesConfig->DataType2[0] != 0xffU) {
                        //; Secondary datatype, video pipe 4, map FS/FE
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0BU, 0x01U);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A2DU, 0x01U);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0DU, pSerdesConfig->DataType2[0]);
                        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0EU, pSerdesConfig->DataType2[0]);
                    } else {
                        // 1-ch with only one datatype
                    }

                    //important !!! Depend on different silicon to disable FS/FE output.
                    //ES1. Reg 0x259 = 0x39 (bit 5 = 1, force VS = 0) for pipe 4
                    //ES2. Reg 0x436 = 0x10 (bit 4 = 1, force VS = 0) for pipe 4
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0436U, 0x10U);

                } else {
                    //not 1ch case
                    //; MIPI controller 1 Concatentation register = b'10000001
                    if(EnabledLinkID == 0xBU) {
                        if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_LINE_INTERLEAVED) {
                            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0971U, 0x07U);
                        } else {
                            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0971U, 0x87U);
                        }
                    } else if((EnabledLinkID == 0x3U) || (EnabledLinkID == 0x7U) || (EnabledLinkID == 0xFU)) {
                        if (pSerdesConfig->VideoAggregationMode == MAX9295_MAX96712_VIDEO_AGGR_LINE_INTERLEAVED) {
                            Data = (UINT8)(EnabledLinkID & 0xFU);
                            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0971U, Data);
                        } else {
                            Data = (UINT8)(EnabledLinkID & 0xFU);
                            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0971U, (Data | 0x80U));
                        }
                    } else {
                        AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] NG! Incorrect EnabledLinkID %d Concatentation register", EnabledLinkID, 0U, 0U, 0U, 0U);
                    }

                }
            }

            //VS debug signal Output on MFP4
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00FAU, 0x10U);

            // -----------------------------------------------------------
            // XHS and XVS GPIO forwarding setup
            // -----------------------------------------------------------
            AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; XVS connects to MAX96712 MFP2?->?MAX9295A MFP7", NULL, NULL, NULL, NULL, NULL);
#ifdef SERIALIZER_BC
            // XVS signal from SoC, DES_MFP2 --> SER_MFP7
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D5U, 0x42U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D3U, 0x84U);
            // XHS signal from SoC, DES_MFP9 --> SER_MFP0
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02C0U, 0x49U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BEU, 0x84U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BFU, 0x20U);

            //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
            //MFP3
            RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02c8, &Data);
            Data = (Data | 0x80U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02c8, Data);
            //MFP5
            RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02ce, &Data);
            Data = (Data | 0x80U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02ce, Data);
            //MFP6
            RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02d1, &Data);
            Data = (Data | 0x80U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02d1, Data);
#else
            //LinkA
            if ((EnabledLinkID & 0x1U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr0;

                // XVS signal from SoC, DES_MFP2 --> SER_MFP7
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D5U, 0x42U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D3U, 0x84U);
                // XHS signal from SoC, DES_MFP9 --> SER_MFP0
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02C0U, 0x49U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BEU, 0x84U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BFU, 0x20U);

                //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
                //MFP3
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02c8, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02c8, Data);
                //MFP5
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02ce, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02ce, Data);
                //MFP6
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02d1, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02d1, Data);

            } else {
                //not LinkA
            }
            //LinkB
            if ((EnabledLinkID & 0x2U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr1;

                // XVS signal from SoC, DES_MFP2 --> SER_MFP7
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D5U, 0x42U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D3U, 0x84U);
                // XHS signal from SoC, DES_MFP9 --> SER_MFP0
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02C0U, 0x49U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BEU, 0x84U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BFU, 0x20U);

                //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
                //MFP3
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02c8, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02c8, Data);
                //MFP5
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02ce, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02ce, Data);
                //MFP6
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02d1, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02d1, Data);
            } else {
                //not LinkB
            }
            //LinkC
            if ((EnabledLinkID & 0x4U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr2;

                // XVS signal from SoC, DES_MFP2 --> SER_MFP7
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D5U, 0x42U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D3U, 0x84U);
                // XHS signal from SoC, DES_MFP9 --> SER_MFP0
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02C0U, 0x49U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BEU, 0x84U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BFU, 0x20U);

                //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
                //MFP3
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02c8, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02c8, Data);
                //MFP5
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02ce, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02ce, Data);
                //MFP6
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02d1, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02d1, Data);
            } else {
                //not LinkC
            }
            //LinkD
            if ((EnabledLinkID & 0x8U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr3;

                // XVS signal from SoC, DES_MFP2 --> SER_MFP7
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D5U, 0x42U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02D3U, 0x84U);
                // XHS signal from SoC, DES_MFP9 --> SER_MFP0
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02C0U, 0x49U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BEU, 0x84U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02BFU, 0x20U);

                //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
                //MFP3
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02c8, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02c8, Data);
                //MFP5
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02ce, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02ce, Data);
                //MFP6
                RetVal |= Max9295_96712_RegRead(VinID, SerAddr, 0x02d1, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x02d1, Data);
            } else {
                //not LinkD
            }
#endif
            AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; Set Internal FSYNC off, GPIO is used for FSYNC, type = GMSL2", NULL, NULL, NULL, NULL, NULL);
            // Set Internal FSYNC off, GPIO is used for FSYNC, type = GMSL2
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x04A0U, 0x08U); //FSYNC_MODE = Frame sync generation is off, GPIO is used for FSYNC, type = GMSL2
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x04AFU, 0x9FU);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0306U, 0x83U); //GPIO_A: MAX96712 MFP2 TX_EN = 1, OUT_DIS = 1, COMP_EN = 0
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x031CU, 0x83U); //GPIO_A: MAX96712 MFP9 TX_EN = 1, OUT_DIS = 1, COMP_EN = 0
            // Config MAX96712 MFP2 to receive external FSYNC signal for each link
            //LinkA
            if (EnabledLinkID == 0x1U) {
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x03AAU, 0x22U); //[TBD] set for 1ch
            } else {
                //not LinkA
            }
            //LinkB
            if ((EnabledLinkID & 0x2U) != 0U) {
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x033DU, 0x22U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0354U, 0x29U); //[TBD] r0x0374U=0x22: 1ch/IMX390
            } else {
                //not LinkB
            }
            //LinkC
            if ((EnabledLinkID & 0x4U) != 0U) {
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0374U, 0x22U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x038AU, 0x29U);
            } else {
                //not LinkC
            }
            //LinkD
            if ((EnabledLinkID & 0x8U) != 0U) {
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x03AAU, 0x22U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x03C1U, 0x29U);
            } else {
                //not LinkD
            }
#ifdef SERIALIZER_BC
            //sensor I2C tanslation for broadcast case
            // -----------------------------------------------------------
            //  sensor I2C tanslation
            // -----------------------------------------------------------
            //LinkA
            if ((EnabledLinkID & 0x1U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr0;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[0];
                SensorAliasID = pSerdesConfig->SensorAliasID[0];
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);
            } else {
                //
            }
            //LinkB
            if ((EnabledLinkID & 0x2U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr1;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[1];
                SensorAliasID = pSerdesConfig->SensorAliasID[1];
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);
            } else {
                //
            }
            //LinkC
            if ((EnabledLinkID & 0x4U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr2;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[2];
                SensorAliasID = pSerdesConfig->SensorAliasID[2];
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);
            } else {
                //
            }
            //LinkD
            if ((EnabledLinkID & 0x8U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr3;
                SensorSlaveID = pSerdesConfig->SensorSlaveID[3];
                SensorAliasID = pSerdesConfig->SensorAliasID[3];
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);
            } else {
                //
            }
#endif

        }
    }
    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_SetSensorClk
 *
 *  @Description:: Set Max9295 reference clock output for image sensor
 *
 *  @Input      ::
 *      VinID:     VIN ID
 *      ChipID:    Chip ID
 *      RClkOut:   Reference clock output
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      INT32 :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max9295_96712_VCC_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkOut)
{
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    UINT8 SlaveID = 0x0U;
    const MAX9295_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX9295_96712_ERR_INVALID_API;
    } else {
        if (ChipID == MX01_MAX9295_A_ID) {
            SlaveID =  pCurSerDesInfo->SerAddr0;
        } else if (ChipID == MX01_MAX9295_B_ID) {
            SlaveID =  pCurSerDesInfo->SerAddr1;
        } else if (ChipID == MX01_MAX9295_C_ID) {
            SlaveID =  pCurSerDesInfo->SerAddr2;
        } else if (ChipID == MX01_MAX9295_D_ID) {
            SlaveID =  pCurSerDesInfo->SerAddr3;
        } else {
            RetVal = MAX9295_96712_ERR_INVALID_API;
        }
    }

    if (RClkOut == MAX9295_96712_RCLK_OUT_FREQ_27M) {
        /* pre-defined clock setting is enabled */
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x0003, 0x07); /* MFP2 to output RCLKOUT */
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x0006, 0xbf);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03f0, 0x53);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03f0, 0x51);
    } else if (RClkOut == MAX9295_96712_RCLK_OUT_FREQ_37P125M) {
        /* pre-defined clock setting is enabled */
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x0003, 0x07); /* MFP2 to output RCLKOUT */
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x0006, 0xbf);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03f0, 0x63);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03f0, 0x61);
    } else if (RClkOut == MAX9295_96712_RCLK_OUT_FREQ_26973027) {
        // settings generated by GMSL SerDes GUI V5.2.3 DPLL Tool
        // # f_in = 25 MHz.  # f_out = 26.973027 MHz.  # f_out - f_out_real = -30 Hz.
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03f0U, 0x02U, 0x43U);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03f4U, 0xd1U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03f5U, 0x00U, 0x0fU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03f0U, 0x00U, 0x03U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1a03U, 0x10U, 0x90U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1a07U, 0x04U, 0xfcU);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x1a08U, 0x45U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1a09U, 0x40U, 0xffU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1a0aU, 0x40U, 0x7fU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1a0bU, 0x60U, 0xe0U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03f0U, 0x01U, 0x01U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x0003U, 0x07U, 0x07U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x0006U, 0x20U, 0x20U);
    } else if (RClkOut == MAX9295_96712_RCLK_OUT_FREQ_37087912) {
        // settings generated by GMSL SerDes GUI V5.2.3 DPLL Tool
        // # f_in = 25 MHz.  # f_out = 37.087912 MHz.  # f_out - f_out_real = 90 Hz.
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03f0U, 0x02U, 0x43U);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03f4U, 0x8fU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03f5U, 0x07U, 0x0fU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03f0U, 0x00U, 0x03U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1a03U, 0x10U, 0x90U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1a07U, 0x04U, 0xfcU);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x1a08U, 0x2fU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1a09U, 0x00U, 0xffU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1a0aU, 0x61U, 0x7fU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1a0bU, 0xa0U, 0xe0U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03f0U, 0x01U, 0x01U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x0003U, 0x07U, 0x07U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x0006U, 0x20U, 0x20U);
    } else {
        RetVal = MAX9295_96712_ERR_ARG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_SetGpioOutput
 *
 *  @Description:: Set Max9295 reference clock output for image sensor
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
UINT32 Max9295_96712_VCC_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT8 Level)
{
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    const MAX9295_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT8 SlaveID = 0U;

    if (ChipID == MX01_MAX96712_ID) {
        SlaveID = pCurSerDesInfo->DesAddr;
    } else if (ChipID == MX01_MAX9295_A_ID) {
        SlaveID = pCurSerDesInfo->SerAddr0;
    } else if (ChipID == MX01_MAX9295_B_ID) {
        SlaveID = pCurSerDesInfo->SerAddr1;
    } else if (ChipID == MX01_MAX9295_C_ID) {
        SlaveID = pCurSerDesInfo->SerAddr2;
    } else if (ChipID == MX01_MAX9295_D_ID) {
        SlaveID = pCurSerDesInfo->SerAddr3;
    } else {
        RetVal = MAX9295_96712_ERR_ARG;
    }

    if (PinID >= AMBA_SBRG_NUM_MAX9295_96712_GPIO_PIN) {
        RetVal = MAX9295_96712_ERR_ARG;
    }

    if(RetVal == MAX9295_96712_ERR_NONE) {
        if(Level == 0U) {
            RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x2cb, 0x24); //need to set low first
            RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x2ca, 0x80);

        } else if(Level == 1U) {
            RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x2ca, 0x90);
        } else {
            RetVal = MAX9295_96712_ERR_ARG;
        }
    } else {
        //MAX9295_96712_ERR
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_RegisterWrite
 *
 *  @Description:: Write Max9295_96712 registers API
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
UINT32 Max9295_96712_VCC_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 Data)
{
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    const MAX9295_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX9295_96712_ERR_INVALID_API;
    } else {

        if (ChipID == MX01_MAX96712_ID) {
            RetVal = Max9295_96712_RegWrite(VinID, pCurSerDesInfo->DesAddr, Addr, Data);
        } else if (ChipID == MX01_MAX9295_A_ID) {
            RetVal = Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr0, Addr, Data);
        } else if (ChipID == MX01_MAX9295_B_ID) {
            RetVal = Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr1, Addr, Data);
        } else if (ChipID == MX01_MAX9295_C_ID) {
            RetVal = Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr2, Addr, Data);
        } else if (ChipID == MX01_MAX9295_D_ID) {
            RetVal = Max9295_96712_RegWrite(VinID, pCurSerDesInfo->SerAddr3, Addr, Data);
        } else {
            RetVal = MAX9295_96712_ERR_ARG;
        }
    }
    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_RegisterRead
 *
 *  @Description:: Read Max9295_96712 registers API
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
UINT32 Max9295_96712_VCC_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 *pData)
{
    const MAX9295_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX9295_96712_ERR_NONE;

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX9295_96712_ERR_INVALID_API;
    } else {

        if (ChipID == MX01_MAX96712_ID) {
            RetVal |= Max9295_96712_RegRead(VinID, pCurSerDesInfo->DesAddr, Addr, pData);
        } else if (ChipID == MX01_MAX9295_A_ID) {
            RetVal |= Max9295_96712_RegRead(VinID, pCurSerDesInfo->SerAddr0, Addr, pData);
        } else if (ChipID == MX01_MAX9295_B_ID) {
            RetVal |= Max9295_96712_RegRead(VinID, pCurSerDesInfo->SerAddr1, Addr, pData);
        } else if (ChipID == MX01_MAX9295_C_ID) {
            RetVal |= Max9295_96712_RegRead(VinID, pCurSerDesInfo->SerAddr2, Addr, pData);
        } else if (ChipID == MX01_MAX9295_D_ID) {
            RetVal = Max9295_96712_RegRead(VinID, pCurSerDesInfo->SerAddr3, Addr, pData);
        } else {
            RetVal |= MAX9295_96712_ERR_ARG;
        }
    }

    return RetVal;
}
