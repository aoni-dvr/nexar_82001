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
#include "AmbaWrap.h"
#include "AmbaPrint.h"

#include "AmbaI2C.h"
#include "AmbaVIN.h"
#include "AmbaSbrg_Max9295_96712.h"

#include "bsp.h"

#define MAX9295_96712_DEBUG //enable if when need to print all sensor setting
#define REDUCE_BOOT_TIME    //reduce delay time during Maxim setting which not confirm by FAE
#define SERIALIZER_BC       //Serializer BC
//#define MAX9295_96712_FULL_LOG          //use Print_Flush to get full log for debug only since it increases much time

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
    [AMBA_VIN_CHANNEL0] = AMBA_MAXIM_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_MAXIM_I2C_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_MAXIM_I2C_CHANNEL_PIP2,
    [AMBA_VIN_CHANNEL3] = AMBA_MAXIM_I2C_CHANNEL_PIP3,
    [AMBA_VIN_CHANNEL4] = AMBA_MAXIM_I2C_CHANNEL_PIP4,
    [AMBA_VIN_CHANNEL5] = AMBA_MAXIM_I2C_CHANNEL_PIP5,
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

    RetVal = AmbaI2C_MasterReadAfterWrite(MAX9295_96712I2cChannel[VinID], AMBA_I2C_SPEED_FAST, 1U,
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
    const UINT16 LockAddr[4] = {0x001AU, 0x000AU, 0x000BU, 0x000CU};
    UINT8 i = 0U;

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
        for (i = 0U; i < MAX96712_NUM_RX_PORT; i ++) {
            if(((EnabledLinkID & ((UINT32)1U << i)) != 0U) && ((Lock & ((UINT32)1U << i)) != ((UINT32)1U << i))) { //Link i
                RetValTmp = Max9295_96712_RegRead(VinID, SlaveID, LockAddr[i], &RxData[i]);
                Count_ary[i] = (UINT8)Count;

                if ((RetValTmp == OK) && ((RxData[i] & 0x08U) == 0x08U)) {
                    Lock |= ((UINT8)1U << i);
                }
            }
        }
        (void)AmbaKAL_TaskSleep(1U);
        Count++;
    }

    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[Max9295_96712] LinkA EnabledLinkID 1/LinkB EnabledLinkID 2/LinkC EnabledLinkID 4/LinkD EnabledLinkID 8", 0U, 0U, 0U, 0U, 0U);
    for (i=0U; i < MAX96712_NUM_RX_PORT; i ++) {
        if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
            if((Lock & ((UINT32)1U << i)) != 0U) {
                AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Max96712 EnabledLinkID %d Reg0x%04x=0x%02x (%d) - GMSL2 locked!", ((UINT32)1U << i), LockAddr[i], RxData[i], Count_ary[i], 0U);
            } else {
                AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Max96712 EnabledLinkID %d Reg0x%04x=0x%02x (%d) - GMSL2 NOT locked!", ((UINT32)1U << i), LockAddr[i], RxData[i], Count_ary[i], 0U);
                RetVal = MAX9295_96712_ERR_SERDES_LINK;
            }
        }
    }

    return RetVal;
}


UINT32 Max9295_96712_Init(UINT32 VinID, UINT16 EnabledLinkID)
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
UINT32 Max9295_96712_GetSerdesStatus(UINT32 VinID, UINT16 EnabledLinkID, MAX9295_96712_SERDES_STATUS_s *pSerdesStatus)
{
    UINT8 RxData = 0;
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    UINT16 i;
    static const UINT16 LinkLockAddr[4] = {0x001aU, 0x000aU, 0x000bU, 0x000cU};
    static const UINT16 VideoLockAddr[4] = {0x0108U, 0x011aU, 0x012cU, 0x013eU};

    if ((EnabledLinkID & 0xFU) == 0U) {
        RetVal = ERR_ARG;
    } else {
        for (i=0U; i < MAX96712_NUM_RX_PORT; i ++) {
            pSerdesStatus[i].LinkLock = MX01_SERDES_STATUS_NOT_SUPPORTED;
            pSerdesStatus[i].VideoLock = MX01_SERDES_STATUS_NOT_SUPPORTED;
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
                    if(Max9295_96712_RegRead(VinID, MX01_IDC_ADDR_MAX96712, VideoLockAddr[i], &RxData) != MAX9295_96712_ERR_NONE) {
                        pSerdesStatus[i].VideoLock = MX01_SERDES_STATUS_NOT_SUPPORTED;
                    } else {
                        if((RxData & 0x40U) == 0x40U) {
                            pSerdesStatus[i].VideoLock = MX01_SERDES_STATUS_OK;
                        } else {
                            pSerdesStatus[i].VideoLock = MX01_SERDES_STATUS_NG;
                        }
                    }
                }
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_Config_Ser
 *
 *  @Description:: Configure Max9295_96712 Serializer
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
static UINT32 Max9295_96712_Config_Ser(UINT32 VinID, const MAX9295_96712_SERDES_CONFIG_s *pSerdesConfig)
{
    const MAX9295_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    UINT16 EnabledLinkID;
    UINT8 SerAddr;
    UINT8 i = 0U;
    UINT8 SerdesI2cAddr[4] = {0U};

    UINT8 Data;
    UINT8 Index = 0U; //array index for NumDataBits, 0/1/2/3/4: 8/10/12/14/16 bit
    const UINT8 Reg0x0312[5] = {0x01U, 0x00U, 0x00U, 0x00U, 0x00U}; //RAW8/10/12/14/16, fix OX03C unexpected sync issue
    const UINT8 Reg0x0313[5] = {0x00U, 0x01U, 0x10U, 0x00U, 0x00U}; //RAW8/10/12/14/16
    const UINT8 Reg0x031C[5] = {0x30U, 0x34U, 0x38U, 0x00U, 0x00U}; //RAW8/10/12/14/16
#ifndef SERIALIZER_BC
    UINT8 SensorSlaveID, SensorAliasID, SensorBCID;
#endif

    if (pSerdesConfig == NULL) {
        RetVal = MAX9295_96712_ERR_ARG;
    } else {
        //Init assignment
        EnabledLinkID = pSerdesConfig->EnabledLinkID;

        SerdesI2cAddr[0] = pCurSerDesInfo->SerAddr0;
        SerdesI2cAddr[1] = pCurSerDesInfo->SerAddr1;
        SerdesI2cAddr[2] = pCurSerDesInfo->SerAddr2;
        SerdesI2cAddr[3] = pCurSerDesInfo->SerAddr3;
#ifndef SERIALIZER_BC
        SensorSlaveID = pSerdesConfig->SensorSlaveID;
        SensorBCID = pSerdesConfig->SensorBCID;
#endif
        Index = (pSerdesConfig->NumDataBits - 8U) / 2U;

#ifdef SERIALIZER_BC
        AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] SERIALIZER_BC", 0U, 0U, 0U, 0U, 0U);
        //Serializer BC slave I2C
        for (i=0U; i < MAX96712_NUM_RX_PORT; i ++) {
            if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
                (void)Max9295_96712_RegWrite(VinID, SerdesI2cAddr[i], 0x0042U, MX01_IDC_ADDR_MAX9295_BC);
                (void)Max9295_96712_RegWrite(VinID, SerdesI2cAddr[i], 0x0043U, SerdesI2cAddr[i]);
            }
        }

        SerAddr = MX01_IDC_ADDR_MAX9295_BC;

        // --------------------------------------------------------------------------------------
        // MAX9295A powers up in GMSL2 mode
        // Config MIPI PHY / Controller
        // Set SER is in 1x4 mode; 4-lane in controller 1 for port A/B/C/D(broadcast); Default lane mapping
        //----------------------------------------------------------------------------------------
        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
        Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum - 1U) << 4U)); // select number of data lanes for Port A/B/C/D(broadcast).
        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U); // Pipe X clock selected Pipe Y clock selected
        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U); // Start Port Pipe X

        //FAE's suggestion
        if((pSerdesConfig->DataType2 != 0xffU) && (EnabledLinkID == 0x1U)) {
            //receive User-defined secondary datatype
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
            Data = ((pSerdesConfig->DataType) | 0x40U);  //bit[6] should be 1;
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);

            //Support to receive secondary datatype for 1ch
            Data = ((pSerdesConfig->DataType2) | 0x40U);  //bit[6] should be 1;
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);
        } else {
            //not support to receive secondary datatype for multi-chan
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
            Data = ((pSerdesConfig->DataType) | 0x40U);  //bit[6] should be 1;
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);
        }
        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0312U, Reg0x0312[Index]); // BPP 8 Double
        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0313U, Reg0x0313[Index]); // BPP 10 Double BPP 12 Double
        RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x031CU, Reg0x031C[Index]); // Soft BPP Pipe X

#else
        AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] No SERIALIZER_BC", 0U, 0U, 0U, 0U, 0U);
        //LinkA
        if ((EnabledLinkID & 0x1U) != 0U) {
            SerAddr = pCurSerDesInfo->SerAddr0;
            SensorAliasID = pSerdesConfig->SensorAliasID[0];

            // --------------------------------------------------------------------------------------
            // MAX9295A powers up in GMSL2 mode
            // Config MIPI PHY / Controller
            // Set SER is in 1x4 mode; 4-lane in controller 1 for port A; Default lane mapping
            //----------------------------------------------------------------------------------------
            //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************         Link A: Serializer MAX9295A         *********************", NULL, NULL, NULL, NULL, NULL);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
            Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum - 1U) << 4U)); // select number of data lanes
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U); // Pipe X clock selected Pipe Y clock selected
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U); // Start Port Pipe X
            //FAE's suggestion
            if((pSerdesConfig->DataType2 != 0xffU) && (EnabledLinkID == 0x1U)) {
                //receive User-defined secondary datatype
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x33U);
            } else {
                //not support to receive secondary datatype for multi-chan
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);
            }

            Data = ((pSerdesConfig->DataType) | 0x40U);  //bit[6] should be 1;
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);

            if((pSerdesConfig->DataType2 != 0xffU) && (EnabledLinkID == 0x1U)) {
                //Support to receive secondary datatype for 1ch
                Data = ((pSerdesConfig->DataType2) | 0x40U);  //bit[6] should be 1;
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0316U, Data);
            } else {
                //not support to receive secondary datatype for multi-ch case
            }
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0312U, Reg0x0312[Index]); // BPP 8 Double
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0313U, Reg0x0313[Index]); // BPP 10 Double BPP 12 Double
            RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x031CU, Reg0x031C[Index]); // Soft BPP Pipe X
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
        //LinkBCD
        for (i = 1U; i < MAX96712_NUM_RX_PORT; i ++) {
            if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
                SerAddr = SerdesI2cAddr[i];
                SensorAliasID = pSerdesConfig->SensorAliasID[i];

                // --------------------------------------------------------------------------------------
                // MAX9295A powers up in GMSL2 mode
                // Config MIPI PHY / Controller
                // Set SER is in 1x4 mode; 4-lane in controller 1 for port B; Default lane mapping
                //----------------------------------------------------------------------------------------
                //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************         Link B: Serializer MAX9295A         *********************", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0330U, 0x00U);
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum - 1U) << 4U)); // select number of data lanes for Port B.
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0331U, Data);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0332U, 0xEEU);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0333U, 0xE4U);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0308U, 0x63U);   // Pipe X clock selected Pipe Y clock selected
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0311U, 0x30U);       // Start Port Pipe X
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0002U, 0x13U);

                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0312U, Reg0x0312[Index]); // BPP 8 Double
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0313U, Reg0x0313[Index]); // BPP 10 Double BPP 12 Double
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x031CU, Reg0x031C[Index]); // Soft BPP Pipe X

                Data = ((pSerdesConfig->DataType) | 0x40U);  //bit[6] should be 1;
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0314U, Data);

                // -----------------------------------------------------------
                //  sensor I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);


            } else {
                //not Link i
            }
        }

#endif
    }
    return RetVal;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_96712_Config_Des
 *
 *  @Description:: Configure Max9295_96712 Deserializer
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
static UINT32 Max9295_96712_Config_Des(UINT32 VinID, const MAX9295_96712_SERDES_CONFIG_s *pSerdesConfig)
{
    const MAX9295_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    UINT16 EnabledLinkID;
    UINT8 DesAddr;
    UINT8 Data;
    UINT8 Index = 0U; //array index for NumDataBits, 0/1/2/3/4: 8/10/12/14/16 bit
    const UINT8 Reg0x0973[5] = {0x02U, 0x04U, 0x01U, 0x00U, 0x00U}; //8/10/12/14/16 bit, fix OX03C unexpected sync issue

    if (pSerdesConfig == NULL) {
        RetVal = MAX9295_96712_ERR_ARG;
    } else {
        //Init assignment
        EnabledLinkID = pSerdesConfig->EnabledLinkID;
        DesAddr = pCurSerDesInfo->DesAddr;
        Index = (pSerdesConfig->NumDataBits - 8U) / 2U; //TBD, will check how to set for different bit for each Link

        // TBD: need to check general case
        // -----------------------------------------------------------
        // Deserializer MIPI Setup
        // -----------------------------------------------------------
        //AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; ************************           Derserialzier MAX96712            *********************", NULL, NULL, NULL, NULL, NULL);
        // pipe X in link B to video pipe 1, pipe X in link A to video pipe 0
        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F0U, 0x40U);

        if(EnabledLinkID == 0x1U) { //A 1ch case
            //; MAX9295A pipe Y in link A --> MAX96712 video pipe 4 and use PHY A (bit[3:0] = 0001'b)
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F2U, 0x51U);
            //; Enable pipe 0 and pipe 4
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F4U, 0x11U);
        } else {
            // pipe X in link D to video pipe 3, pipe X in link C to video pipe 2
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F1U, 0xC8U);
            // Enable pipe 01(2ch)/0-2(3ch)/0-3(4ch)
            Data = (UINT8)(EnabledLinkID & 0xFU);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00F4U, Data);
        }
        //MIPI PHY Setting
        //Set Des in 2x4 mode
        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08A0U, 0x04U);
        //Set Lane Mapping for 4-lane port A
        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08A3U, 0xE4U);
        //RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08A4U, 0xE4U);
        //Set D-PHY lane num
        //RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090AU, ((pSerdesConfig->CSITxLaneNum[0]-1U) << 6U));
        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x094AU, ((pSerdesConfig->CSITxLaneNum[0]-1U) << 6U));
        //RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x098AU, ((pSerdesConfig->CSITxLaneNum[0]-1U) << 6U));
        //RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x09CAU, ((pSerdesConfig->CSITxLaneNum[0]-1U) << 6U));
        //Turn on MIPI PHYs PHY0 and PHY1
        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x08A2U, 0x30U); // MIPI_PHY:MIPI_PHY2, [7:4] = 0b1111 (MIPI PHY 0-1-2-3 Enable)                                                                     //   [7:4] = 0b1111 (MIPI PHY 0-1-2-3 Enable)
        //Put DPLL in reset while changing MIPI data rate
        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x1D00U, 0xF4U);
        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x1E00U, 0xF4U);

        // CSI bit rate per lane, CSITxSpeed * 100 MHz
        Data = 0x20U | pSerdesConfig->CSITxSpeed[0];
        //RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0415U, Data);
        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0418U, Data);
        //RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x041BU, Data);
        //RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x041EU, Data);

        //release reset to DPLL1 and DPLL2
        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x1D00U, 0xF5U);
        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x1E00U, 0xF5U);

        if(EnabledLinkID == 1U) {
            //; Video Pipe to MIPI Controller Mapping
            //; RAW12, video pipe 0
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090BU, 0x07U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x092DU, 0x15U);

            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090DU, pSerdesConfig->DataType);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090EU, pSerdesConfig->DataType);

            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x090FU, 0x00U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0910U, 0x00U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0911U, 0x01U);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0912U, 0x01U);

            if(pSerdesConfig->DataType2 != 0xffU) {
                //; Secondary datatype, video pipe 4, map FS/FE
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0BU, 0x01U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A2DU, 0x01U);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0DU, pSerdesConfig->DataType2);
                RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0A0EU, pSerdesConfig->DataType2);
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
            Data = (UINT8)(EnabledLinkID & 0xFU);
            RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0971U, Data);

        }
        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0973U, Reg0x0973[Index]);
        //VS debug signal Output on MFP4
        RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x00FAU, 0x10U);
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
UINT32 Max9295_96712_Config(UINT32 VinID, const MAX9295_96712_SERDES_CONFIG_s *pSerdesConfig)
{
    const MAX9295_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    UINT16 EnabledLinkID;
#ifdef SERIALIZER_BC
    UINT8 SensorSlaveID, SensorAliasID, SensorBCID;
#endif
    UINT8 SerAddr, DesAddr;
    UINT8 Data;
    UINT8 SerdesI2cAddr[4] = {0U};
    const UINT16 DesGpioAddr[4][2] = {
        {0xFFFFU, 0xFFFFU}, //not set LinkA
        {0x033DU, 0x0354U},
        {0x0374U, 0x038AU},
        {0x03AAU, 0x03C1U},
    };
    UINT8 i = 0U;

    if (pSerdesConfig == NULL) {
        RetVal = MAX9295_96712_ERR_ARG;
    } else {
        if (pCurSerDesInfo->InitDone == 0U) {
            RetVal = MAX9295_96712_ERR_INVALID_API;
        } else {
            EnabledLinkID = pSerdesConfig->EnabledLinkID;
            DesAddr = pCurSerDesInfo->DesAddr;
#ifdef SERIALIZER_BC
            SensorSlaveID = pSerdesConfig->SensorSlaveID;
            SensorBCID = pSerdesConfig->SensorBCID;
#endif

            SerdesI2cAddr[0] = pCurSerDesInfo->SerAddr0;
            SerdesI2cAddr[1] = pCurSerDesInfo->SerAddr1;
            SerdesI2cAddr[2] = pCurSerDesInfo->SerAddr2;
            SerdesI2cAddr[3] = pCurSerDesInfo->SerAddr3;

            if((EnabledLinkID == 0x3U) || (EnabledLinkID == 0x7U) || (EnabledLinkID == 0xbU) || (EnabledLinkID == 0xfU)) {
                AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Config (EnabledLinkID: 0x%02x)... transfer RAW12 only(multi-ch)", EnabledLinkID, 0U, 0U, 0U, 0U);
            } else if(EnabledLinkID == 0x1U) {
                if(pSerdesConfig->DataType2 != 0xffU) {
                    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Config (EnabledLinkID: 0x%02x)... transfer RAW%d+OB (1-ch)", EnabledLinkID, pSerdesConfig->NumDataBits, 0U, 0U, 0U);
                } else {
                    AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] Config (EnabledLinkID: 0x%02x)... transfer RAW%d only(1-ch)", EnabledLinkID, pSerdesConfig->NumDataBits, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(MAX9295_96712_MODULE_ID, "[MAX9295_96712] NG! Incorrect SensorID 0x%02x", EnabledLinkID, 0U, 0U, 0U, 0U);
                RetVal = MAX9295_96712_ERR_ARG;
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
            }
#endif

            //I2C slave address translation

            AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; I2C slave address translation", NULL, NULL, NULL, NULL, NULL);
            for (i = 0U; i < MAX96712_NUM_RX_PORT; i ++) {
                if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
                    Data = (0xF0U) | ((UINT8)1U << i);
                    //Enable Link X Only and change Link X MAX9295A slave address to
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, 0x0006U, Data);
#ifndef REDUCE_BOOT_TIME
                    (void)AmbaKAL_TaskSleep(100U);
#else
                    (void)AmbaKAL_TaskSleep(45U);
#endif
                    RetVal |= Max9295_96712_RegWrite(VinID, MX01_IDC_ADDR_MAX9295_DEFAULT, 0x0000U, (UINT8)SerdesI2cAddr[i]);

                }
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

            RetVal |= Max9295_96712_Config_Ser(VinID, pSerdesConfig);

            RetVal |= Max9295_96712_Config_Des(VinID, pSerdesConfig);

            // -----------------------------------------------------------
            // XHS and XVS GPIO forwarding setup
            // -----------------------------------------------------------
            AmbaPrint_ModulePrintStr5(MAX9295_96712_MODULE_ID, "; XVS connects to MAX96712 MFP2?->?MAX9295A MFP7", NULL, NULL, NULL, NULL, NULL);
#ifdef SERIALIZER_BC
            SerAddr = MX01_IDC_ADDR_MAX9295_BC;
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
            for (i=0U; i < MAX96712_NUM_RX_PORT; i ++) {
                if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
                    SerAddr = SerdesI2cAddr[i];

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
                    //not Link i
                }
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

            for (i = 1U; i < MAX96712_NUM_RX_PORT; i ++) {
                if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, DesGpioAddr[i][0U], 0x22U);
                    RetVal |= Max9295_96712_RegWrite(VinID, DesAddr, DesGpioAddr[i][1U], 0x29U);
                }

            }
#ifdef SERIALIZER_BC
            //sensor I2C tanslation for broadcast case
            // -----------------------------------------------------------
            //  sensor I2C tanslation
            // -----------------------------------------------------------

            for (i = 0U; i < MAX96712_NUM_RX_PORT; i ++) {
                if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
                    SerAddr = SerdesI2cAddr[i];
                    SensorAliasID = pSerdesConfig->SensorAliasID[i];
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0044, SensorBCID);
                    RetVal |= Max9295_96712_RegWrite(VinID, SerAddr, 0x0045, SensorSlaveID);
                } else {
                    //
                }
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
UINT32 Max9295_96712_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkOut)
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
        // settings generated by GMSL SerDes GUI V5.4.0 DPLL Tool, Output RCLK_ALT (MFP #2)
        // # f_in = 25 MHz.  # f_out = 27 MHz.  # f_out - f_out_real = -30 Hz.
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x02, 0x43);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03F4, 0xEC);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F5, 0x01, 0x0F);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x00, 0x03);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A03, 0x10, 0x90);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A07, 0x04, 0xFC);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x1A08, 0x45);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A09, 0x40, 0xFF);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A0A, 0x40, 0x7F);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A0B, 0x60, 0xE0);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x01, 0x01);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x0003, 0x07, 0x07);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x0006, 0x20, 0x20);
    } else if (RClkOut == MAX9295_96712_RCLK_OUT_FREQ_37P125M) {
        /* pre-defined clock setting is enabled */
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x0003, 0x07); /* MFP2 to output RCLKOUT */
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x0006, 0xbf);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03f0, 0x63);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03f0, 0x61);
    } else if (RClkOut == MAX9295_96712_RCLK_OUT_FREQ_26973027) {
        // settings generated by GMSL SerDes GUI V5.4.0 DPLL Tool, Output RCLK_ALT (MFP #2)
        // # f_in = 25 MHz.  # f_out = 26.973027 MHz.  # f_out - f_out_real = -30 Hz.
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x02, 0x43);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03F4, 0xD1);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F5, 0x00, 0x0F);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x00, 0x03);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A03, 0x10, 0x90);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A07, 0x04, 0xFC);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x1A08, 0x45);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A09, 0x40, 0xFF);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A0A, 0x40, 0x7F);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A0B, 0x60, 0xE0);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x01, 0x01);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x0003, 0x07, 0x07);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x0006, 0x20, 0x20);
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
    } else if (RClkOut == MAX9295_96712_RCLK_OUT_FREQ_24M) {
       /* pre-defined clock setting is disabled */
       RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03F1, 0x05); /* MFP2 to output RCLKOUT */
       RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03F0, 0x12);
       RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03F4, 0x0A);
       RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03F5, 0x07);
       RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03F0, 0x10);
       RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x1A03, 0x12);
       RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x1A07, 0x04);
       RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x1A08, 0x3D);
       RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x1A09, 0x40);
       RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x1A0A, 0xC0);
       RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x1A0B, 0x7F);
       RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03F0, 0x11);
    } else if (RClkOut == MAX9295_96712_RCLK_OUT_FREQ_23976024) {
        // settings generated by GMSL SerDes GUI V6.3.9 DPLL Tool
        // # f_in = 25 MHz.  # f_out = 23.976024 MHz.  # f_out - f_out_real = -16 Hz.
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F0U, 0x02U, 0x43U);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x03F4U, 0x0FU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F5U, 0x06U, 0x0FU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F0U, 0x00U, 0x03U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A03U, 0x10U, 0x90U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A07U, 0x04U, 0xFCU);
        RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, 0x1A08U, 0x3DU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A09U, 0x40U, 0xFFU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A0AU, 0x40U, 0x7FU);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x1A0BU, 0x60U, 0xE0U);
        RetVal |= Max9295_96712_RegReadModifyWrite(VinID, SlaveID, 0x03F0U, 0x01U, 0x01U);
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
UINT32 Max9295_96712_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT8 Level)
{
    UINT32 RetVal = MAX9295_96712_ERR_NONE;
    UINT16 Addr = 0U; /* register address */
    MAX9295_96712_GPIO_A_REG_s GpioAReg = {0};
    MAX9295_96712_GPIO_B_REG_s GpioBReg = {0};
    const MAX9295_96712_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT8 SlaveID = 0U, RegAVal, RegBVal;

    if (ChipID == MX01_MAX96712_ID) {
        Addr = (UINT16)0x300 + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->DesAddr;
    } else if (ChipID == MX01_MAX9295_A_ID) {
        Addr = (UINT16)0x2be + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->SerAddr0;
    } else if (ChipID == MX01_MAX9295_B_ID) {
        Addr = (UINT16)0x2be + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->SerAddr1;
    } else if (ChipID == MX01_MAX9295_C_ID) {
        Addr = (UINT16)0x2be + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->SerAddr2;
    } else if (ChipID == MX01_MAX9295_D_ID) {
        Addr = (UINT16)0x2be + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->SerAddr3;
    } else {
        RetVal = MAX9295_96712_ERR_ARG;
    }

    if (PinID >= AMBA_SBRG_NUM_MAX9295_96712_GPIO_PIN) {
        RetVal = MAX9295_96712_ERR_ARG;
    }

    if (RetVal == MAX9295_96712_ERR_NONE) {

       if (PinID >= AMBA_SBRG_NUM_MAX9295_96712_GPIO_PIN) {
           RetVal = MAX9295_96712_ERR_ARG;
       } else {
           RetVal |= Max9295_96712_RegRead(VinID, SlaveID, Addr, &RegAVal);
           RetVal |= Max9295_96712_RegRead(VinID, SlaveID, Addr + 1U, &RegBVal);
           if (RetVal == MAX9295_96712_ERR_NONE) {
               if (AmbaWrap_memcpy(&GpioAReg, &RegAVal, sizeof(RegAVal)) != ERR_NONE) {
                   RetVal = MAX9295_96712_ERR_UNEXPECTED;
               }
               if (AmbaWrap_memcpy(&GpioBReg, &RegBVal, sizeof(RegBVal)) != ERR_NONE) {
                   RetVal = MAX9295_96712_ERR_UNEXPECTED;
               }

               GpioBReg.GpioTxId = (UINT8)PinID;
               GpioBReg.OutType = 1U;
               GpioBReg.PullUpDnSel = 0U;

               GpioAReg.GpioOutDis = 0U;
               GpioAReg.GpioTxEn = 0U;
               GpioAReg.GpioRxEn = 0U;
               if (Level == 0U) {
                   GpioAReg.GpioOut = 0U;
               } else {
                   GpioAReg.GpioOut = 1U;
               }
               GpioAReg.TxCompEn = 0U;
               GpioAReg.TxPrio = 0U;
               GpioAReg.ResCfg = 1U;

               if (AmbaWrap_memcpy(&RegAVal, &GpioAReg, sizeof(RegAVal)) != ERR_NONE) {
                   RetVal = MAX9295_96712_ERR_UNEXPECTED;
               }
               if (AmbaWrap_memcpy(&RegBVal, &GpioBReg, sizeof(RegBVal)) != ERR_NONE) {
                   RetVal = MAX9295_96712_ERR_UNEXPECTED;
               }
               RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, Addr, RegAVal);
               RetVal |= Max9295_96712_RegWrite(VinID, SlaveID, Addr + 1U, RegBVal);
           } else {
               RetVal = MAX9295_96712_ERR_ARG;
           }
        }
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
UINT32 Max9295_96712_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 Data)
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
UINT32 Max9295_96712_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 *pData)
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
