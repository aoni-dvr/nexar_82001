/**
 *  @file AmbaSbrg_Max9295_9296.c
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
 *  @details MAX9295 & MAX9296 driver APIs
 *
 */

//#include "AmbaSSP.h"

#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"

#include "AmbaI2C.h"
#include "AmbaVIN.h"
#include "AmbaSbrg_Max9295_9296_REMAP.h"

#include "bsp.h"

#define MAX9295_9296_DEBUG

typedef struct {
    UINT32 InitDone;
    UINT8 DesAddr;          /* Max9296 I2C Slave Addr */
    UINT8 SerAddr0;         /* Distinguishable Max9295 LinkA I2C Slave Addr */
    UINT8 SerAddr1;         /* Distinguishable Max9295 LinkB I2C Slave Addr */
} MAX9295_9296_SERDES_INFO_s;

static UINT32 MAX9295_9296I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
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

static MAX9295_9296_SERDES_INFO_s MaxSerDesInfo[AMBA_NUM_VIN_CHANNEL] = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_9296_Remap_RegWrite
 *
 *  @Description:: Write Max9295_9296 registers
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
static UINT32 Max9295_9296_Remap_RegWrite(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 Data)
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

    RetVal = AmbaI2C_MasterWrite(MAX9295_9296I2cChannel[VinID], AMBA_I2C_SPEED_FAST_PLUS, &I2cConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    /* print using MAXIM GMSL GUI's write command format */
#ifdef MAX9295_9296_DEBUG
    AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296 VinID%d Ch%d] WIB %02X %04X %02X", VinID, MAX9295_9296I2cChannel[VinID], SlaveID, Addr, Data);
#endif

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_9296_RegRead
 *
 *  @Description:: Read Max9295_9296 registers
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
static UINT32 Max9295_9296_RegRead(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 *pRxData)
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

    RetVal = AmbaI2C_MasterReadAfterWrite(MAX9295_9296I2cChannel[VinID], AMBA_I2C_SPEED_FAST_PLUS, 1U,
                                          &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "VinID %d SlaveID 0x%02x Addr 0x%04x I2C does not work!!!!!", VinID, SlaveID, Addr, 0U, 0U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_9296_WaitLockStatus
 *
 *  @Description:: Wait for Max9295_9296 link to be established
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
static UINT32 Max9295_9296_WaitLockStatus(UINT32 VinID, UINT8 SlaveID, UINT32 TimeOut)
{
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    UINT32 Count = 0U;
    UINT8 RxData = 0x0U;

    AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "Max9295_9296_WaitLockStatus: SlaveID 0x%02x", SlaveID, 0U, 0U, 0U, 0U);

    while (((RxData & 0x08U) == 0x0U) && (Count < TimeOut)) {
        RxData = 0x0U;

        RetVal = Max9295_9296_RegRead(VinID, SlaveID, 0x0013U, &RxData);
        if (RetVal != MAX9295_9296_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] VinID %d Max9295_9296_WaitLockStatus fail", VinID, 0, 0U, 0U, 0U);
            break;
        }

        (void) AmbaKAL_TaskSleep(1);
        Count++;
    }

    if (Count == TimeOut) {
        AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Max9296 Reg0x13=0x%02x (%d) - GMSL2 NOT locked!", RxData, Count, 0U, 0U, 0U);
        RetVal = MAX9295_9296_ERR_SERDES_LINK;
    }

    return RetVal;
}

UINT32 Max9295_9296_Remap_DeInit(UINT32 VinID)
{
    MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    pCurSerDesInfo->InitDone = 0;

    return MAX9295_9296_ERR_NONE;
}

UINT32 Max9295_9296_Remap_Init(UINT32 VinID, UINT16 EnabledLinkID)
{
    MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    static UINT8 Init = 0;
    UINT8 Data, Data1;
    static UINT8 IdcAddrMax9296[AMBA_NUM_VIN_CHANNEL] = {
        MX00_IDC_ADDR_MAX9296_0,
        //VC: VIN1-3
        0,
        0,
        0,
        MX00_IDC_ADDR_MAX9296_4,
        //VC: VIN5-7
        0,
        0,
        0,
        MX00_IDC_ADDR_MAX9296_8,
        //VC: VIN9-10
        0,
        0,
        MX00_IDC_ADDR_MAX9296_11,
        //VC: VIN12-13
        0,
        0,
    };

    static UINT8 IdcAddrMax9295[AMBA_NUM_VIN_CHANNEL][2] = {
        {MX00_IDC_ADDR_MAX9295_0_A, MX00_IDC_ADDR_MAX9295_0_B},
        //VC: VIN1-3
        {0},
        {0},
        {0},
        {MX00_IDC_ADDR_MAX9295_4_A, MX00_IDC_ADDR_MAX9295_4_B},
        //VC: VIN5-7
        {0},
        {0},
        {0},
        {MX00_IDC_ADDR_MAX9295_8_A, MX00_IDC_ADDR_MAX9295_8_B},
        //VC: VIN9-10
        {0},
        {0},
        {MX00_IDC_ADDR_MAX9295_11_A, MX00_IDC_ADDR_MAX9295_11_B},
        //VC: VIN12-13
        {0},
        {0},
    };

    if (pCurSerDesInfo->InitDone == 0U) {
        AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296_REMAP] Init", NULL, NULL, NULL, NULL, NULL);
    } else {
        AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296_REMAP] Init for Vin lost Recovery", NULL, NULL, NULL, NULL, NULL);
    }

    pCurSerDesInfo->DesAddr = IdcAddrMax9296[VinID];
    pCurSerDesInfo->SerAddr0 = IdcAddrMax9295[VinID][0];
    pCurSerDesInfo->SerAddr1 = IdcAddrMax9295[VinID][1];

    AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] DesAddr 0x%x, SerAddr0 0x%x, SerAddr1 0x%x", pCurSerDesInfo->DesAddr, pCurSerDesInfo->SerAddr0, pCurSerDesInfo->SerAddr1, 0U, 0U);
    //Check locked
    (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
    (void) Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200);

    //Check MAX9296 version
    if((Max9295_9296_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0d, &Data) == I2C_ERR_NONE)) {
        if(Max9295_9296_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0e, &Data1) == I2C_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Check MAX9296 Chip version r0xd=0x%x, r0xe=0x%x", Data, Data1, 0U, 0U, 0U);
            if((Data == 0x94U) && ((Data1 == 0U) || (Data1 == 1U) || (Data1 == 2U) || (Data1 == 255U) || (Data1 == 4U) || (Data1 == 5U) || (Data1 == 6U) || (Data1 == 7U))) {
                if(Data1 == 0x5U) {
                    AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Supported ES4.0 MAX9296", 0U, 0U, 0U, 0U, 0U);
                } else if(Data1 == 0x6U) {
                    AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Supported ES5.0 MAX9296", 0U, 0U, 0U, 0U, 0U);
                } else if(Data1 == 0x7U) {
                    AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Supported Production Samples MAX9296(new)", 0U, 0U, 0U, 0U, 0U);

                    //PHYA/PHB optimization before one shot reset for Productions Sample by FAE's suggestion(19/06)
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1458, 0x28);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1459, 0x68);

                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1558, 0x28);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1559, 0x68);

                } else {
                    AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Unknown version MAX9296", 0U, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Not in MAX9296 Supported List", 0U, 0U, 0U, 0U, 0U);
            }
        } else {
            (void)Max9295_9296_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0e, &Data1);
            AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Check MAX9296 Chip version r0xd=0x%x, r0xe=0x%x", Data, Data1, 0U, 0U, 0U);
            AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Not in MAX9296 Supported List", 0U, 0U, 0U, 0U, 0U);
        }
    }

    //Need to check vin recovery for multi MAX9296
    if (Init == 0U) {
        //turn off all Vin0/Vin1 MAX9295 to avoid conflict i2c slave addr
        AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] turn off all Vin0/Vin1 MAX9295 to avoid conflict i2c slave addr", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Please skip \"I2C does not work\" msg if not connect to those MAX9296 i2c SlaveID", NULL, NULL, NULL, NULL, NULL);

        for (UINT32 i = 0U; i < AMBA_NUM_VIN_CHANNEL; i ++) {
            if(Max9295_9296_RegRead(i, IdcAddrMax9296[i], 1, &Data) == MAX9295_9296_ERR_NONE) {
                Data |= (UINT8)0x10; // set DIS_REM_CC for Vin0/Vin2 MAX9296
                RetVal |= Max9295_9296_Remap_RegWrite(i, IdcAddrMax9296[i], 1, Data);
            } else {
                //No vin0/vin2 MAX9296
            }
        }

        Init = 1U;
    }
    AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Turn on Vin%d Max9295 slave addr", VinID, 0U, 0U, 0U, 0U);
    //turn on Config vin MAX9295
    RetVal |= Max9295_9296_RegRead(VinID, pCurSerDesInfo->DesAddr, 1, &Data);
    Data &= (UINT8)0xEF; // clear DIS_REM_CC
    RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->DesAddr, 1, Data);

    AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Please skip \"i2c doesn't work\" for 1st bootup case or not connect to LinkA/B MAX9295. (not reboot)", NULL, NULL, NULL, NULL, NULL);

    if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200U) != MAX9295_9296_ERR_NONE) {
        RetVal = MAX9295_9296_ERR_SERDES_LINK;
    } else {
        UINT32 Rval0 = Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0000U, &Data);
        UINT32 Rval1 = Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0000U, &Data);
        // -----------------------------------------------------------
        // Reset parts
        // -----------------------------------------------------------
        //if modified MAX9295 i2c slave addr existed =>  reset MAX9295 for all (not first boot-up case)
        if((Rval0 == MAX9295_9296_ERR_NONE) || (Rval1 == MAX9295_9296_ERR_NONE)) {
            AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] reset MAX9295 for not power off/on case", NULL, NULL, NULL, NULL, NULL);
            if (Rval0 == MAX9295_9296_ERR_NONE) {
                ///set MFP2 as GPIO low to make CFG1 works fine when reset MAX9295
                //disable MAX9295 clk_si
                (void) Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x03, 0x3U);
                //set MFP2 as GPIO low
                (void) Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x2c4, 0x80U);
                //MAX9295 reset all
                (void) Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x10, 0x81U); //TBD: 0x81 for LinkA only by FAE
            }
            if(Rval1 == MAX9295_9296_ERR_NONE) {
                ///set MFP2 as GPIO low to make CFG1 works fine when reset MAX9295
                //disable MAX9295 clk_si
                (void) Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x03, 0x3U);
                //set MFP2 as GPIO low
                (void) Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x2c4, 0x80U);
                //MAX9295 reset all
                (void) Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x10, 0x81U); //TBD: 0x81 for LinkA only by FAE
            }

            //Check locked
            (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
            (void) Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200);
        } else {
            AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] not reset MAX9295 for 1st bootup case", NULL, NULL, NULL, NULL, NULL);
        }

        AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Please skip \"i2c doesn't work\" msg if not do reboot without power off/on Max9296.", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Max9295_9296_Remap_Init should return OK for this case", NULL, NULL, NULL, NULL, NULL);
        if (EnabledLinkID == 0x1U) {
            AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] One-shot reset for Link A", NULL, NULL, NULL, NULL, NULL);
            RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x21U);
            //wait 25ms then check locked
            (void)AmbaKAL_TaskSleep(25U);
            if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200U) != MAX9295_9296_ERR_NONE) {
                RetVal = MAX9295_9296_ERR_SERDES_LINK;
            } else {
                if(Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0000U, &Data) != MAX9295_9296_ERR_NONE) {
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, MX00_IDC_ADDR_MAX9295_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr0);
                } else {
                    //LinkA MAX9295 i2c transmission done
                }
            }
        } else if (EnabledLinkID == 0x2U) {
            AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] One-shot reset for Link B", NULL, NULL, NULL, NULL, NULL);
            RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x22U);
            //wait 25ms then check locked
            (void)AmbaKAL_TaskSleep(25U);
            if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200U) != MAX9295_9296_ERR_NONE) {
                RetVal = MAX9295_9296_ERR_SERDES_LINK;
            } else {
                if(Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0000U, &Data) != MAX9295_9296_ERR_NONE) {
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, MX00_IDC_ADDR_MAX9295_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr1);
                } else {
                    //LinkB MAX9295 i2c transmission done
                }
            }
        } else { /* if (EnabledLinkID == 0x3) */
            AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] One-shot reset for Link A", NULL, NULL, NULL, NULL, NULL);
            RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x21U);
            //wait 25ms then check locked
            (void)AmbaKAL_TaskSleep(25U);
            if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200U) != MAX9295_9296_ERR_NONE) {
                RetVal = MAX9295_9296_ERR_SERDES_LINK;
            } else {
                if(Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0000U, &Data) != MAX9295_9296_ERR_NONE) {
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, MX00_IDC_ADDR_MAX9295_DEFAULT, 0x0000U, pCurSerDesInfo->SerAddr0);
                } else {
                    //LinkA MAX9295 i2c transmission done
                }
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x006BU, 0x16U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x0073U, 0x17U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x007BU, 0x36U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x0083U, 0x36U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x0093U, 0x36U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x009BU, 0x36U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x00A3U, 0x36U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x00ABU, 0x36U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x008BU, 0x36U);

                AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] One-shot reset for Link B", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x22U);
                //Check locked
                (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
                if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200) != MAX9295_9296_ERR_NONE) {
                    RetVal = MAX9295_9296_ERR_SERDES_LINK;
                } else {
                    if(Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0000U, &Data) != MAX9295_9296_ERR_NONE) {
                        RetVal |= Max9295_9296_Remap_RegWrite(VinID, MX00_IDC_ADDR_MAX9295_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr1);
                    } else {
                        //LinkB MAX9295 i2c transmission done
                    }
                }
                AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] one-shot reset and deserializer splitter mode", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x23U);
                //Check locked
                (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
                if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200) != MAX9295_9296_ERR_NONE) {
                    RetVal = MAX9295_9296_ERR_SERDES_LINK;
                }
            }
        }
    }

    //TBD: check slave mode
    if (EnabledLinkID == 0x1U) {
        //TBD: check config vin MAX9295 slave addr
        RetVal |= Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0, &Data);
        AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[TBD] VinID %d SlaveID 0x%x -> 0x%x", VinID, pCurSerDesInfo->SerAddr0, Data, 0U, 0U);
    } else if (EnabledLinkID == 0x2U) {
        //TBD: check config vin MAX9295 slave addr
        RetVal |= Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0, &Data);
        AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[TBD] VinID %d SlaveID 0x%x -> 0x%x", VinID, pCurSerDesInfo->SerAddr1, Data, 0U, 0U);
    } else if (EnabledLinkID == 0x3U) {
        //TBD: check config vin MAX9295 slave addr
        RetVal |= Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0, &Data);
        RetVal |= Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0, &Data1);
        AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[TBD] VinID %d SlaveID 0x%x/0x%x -> 0x%x/0x%x", VinID, pCurSerDesInfo->SerAddr0, pCurSerDesInfo->SerAddr1, Data, Data1);
    } else {
        RetVal = MAX9295_9296_ERR_ARG;
    }

    if(RetVal == MAX9295_9296_ERR_NONE) {
        pCurSerDesInfo->InitDone = 1U;
    } else {
        //not InitDone
    }


#if 0
    // -----------------------------------------------------------
    // Reset parts
    // -----------------------------------------------------------
    // One-shot reset is only needed if you change operation mode, for example link rate.
    // Or if you see something abnormal then one-shot may fix most of sequencing issues.
    // By default, MAX9296A is already in single link mode. You don't need to one-sho
    // reset unless you change it to other mode for example splitter mode.
    AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] one-shot reset", EnabledLinkID);
    Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010, 0x31);

    if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 100) != OK)
        return NG;
#endif

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max9295_9296_Remap_Config
 *
 *  @Description:: Configure Max9295_9296
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
UINT32 Max9295_9296_Remap_Config(UINT32 VinID, const MAX9295_9296_SERDES_CONFIG_s *pSerdesConfig)
{
    const MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    UINT16 EnabledLinkID;
    UINT8 SensorSlaveID, SensorAliasID;
    UINT8 SerAddr, DesAddr;
    UINT8 Data;

    if (pSerdesConfig == NULL) {
        RetVal = MAX9295_9296_ERR_ARG;
    } else {
        if (pCurSerDesInfo->InitDone == 0U) {
            RetVal = MAX9295_9296_ERR_INVALID_API;
        } else {
            EnabledLinkID   = pSerdesConfig->EnabledLinkID;
            SensorSlaveID   = pSerdesConfig->SensorSlaveID;
            DesAddr         = pCurSerDesInfo->DesAddr;

            AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296_REMAP] Config (EnabledLinkID: 0x%02x)...", EnabledLinkID, 0U, 0U, 0U, 0U);

            if ((EnabledLinkID & 0x1U) != 0U) {
                /*---------------------------------------------
                 * [LINK A only or in reserve splitter mode]
                 * SER: MIPI Port B -> Pipe X (TX Stream ID=1)
                 * DES: Pipe Y (RX Stream ID=1) -> Controller 1
                 ----------------------------------------------*/
                SerAddr = pCurSerDesInfo->SerAddr0;
                SensorAliasID = pSerdesConfig->SensorAliasID[0];
                // -----------------------------------------------------------
                // Video Pipes setup
                // -----------------------------------------------------------
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0312, 0x01);
                Data = 0x20U | (pSerdesConfig->NumDataBits & 0x1fU);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x031C, Data);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0473, 0x10);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0102, 0x0E);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0112, 0x23);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0308, 0x71);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0311, 0x10);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0100, 0x60);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0101, 0x50);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0102, 0x0C);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0053, 0x11);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0057, 0x10);

                // -----------------------------------------------------------
                // XHS and XVS GPIO forwarding setup
                // -----------------------------------------------------------
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0003, 0x40);
                // XVS signal from SoC, DES_MFP5 --> SER_MFP7
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x02BF, 0xA3);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02D3, 0xA4);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02D4, 0x27);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02D5, 0x45);
                // XHS signal from SoC, DES_MFP0 --> SER_MFP0
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x02B0, 0xA3);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02BE, 0x24);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02BF, 0x20);

                // -----------------------------------------------------------
                // Serializer MIPI Setup
                // -----------------------------------------------------------
                // Set MIPI_RX registers number of lanes 1x4 mode
                (void) Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0330, 0x00);
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum - 1U) << 4U)); // select number of data lanes for Port B.
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0331, Data);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0332, 0xEE);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0333, 0xE4);

                // -----------------------------------------------------------
                //  IMX224 I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);

                //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
                //MFP3
                RetVal |= Max9295_9296_RegRead(VinID, SerAddr, 0x02c8, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02c8, Data);
                //MFP5
                RetVal |= Max9295_9296_RegRead(VinID, SerAddr, 0x02ce, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02ce, Data);
                //MFP6
                RetVal |= Max9295_9296_RegRead(VinID, SerAddr, 0x02d1, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02d1, Data);
            }
            if ((EnabledLinkID & 0x2U) != 0U) {
                SerAddr = pCurSerDesInfo->SerAddr1;
                SensorAliasID = pSerdesConfig->SensorAliasID[1];
                // -----------------------------------------------------------
                // Video Pipes setup
                // -----------------------------------------------------------
                if (EnabledLinkID == 0x2U) {
                    /*---------------------------------------------
                     * [LINK B only]
                     * SER: MIPI Port B -> Pipe X (TX Stream ID=1)
                     * DES: Pipe Y (RX Stream ID=1) -> Controller 1
                     ----------------------------------------------*/
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0312, 0x01);
                    Data = 0x20U | (pSerdesConfig->NumDataBits & 0x1fU);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x031C, Data);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0473, 0x10);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0102, 0x0E);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0112, 0x23);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0308, 0x71); /* Video Pipe X from Port B */
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0311, 0x10);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0100, 0x60);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0101, 0x50);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0102, 0x0C);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0053, 0x11); /* Stream ID for packets from Pipe X is 1 */
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0057, 0x10);
                } else { /* if (EnabledLinkID== 0x3) */
                    /*---------------------------------------------
                     * [LINK B in reverse splitter mode]
                     * SER: MIPI Port B -> Pipe X (TX Stream ID=2)
                     * DES: Pipe Z (RX Stream ID=2) -> Controller 2
                     ----------------------------------------------*/
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0312, 0x01);
                    Data = 0x20U | (pSerdesConfig->NumDataBits & 0x1fU);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x031C, Data);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x04B3, 0x10);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0102, 0x0E);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0124, 0x23);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0308, 0x71); /* Video Pipe X from Port B */
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0311, 0x10);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0100, 0x60);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0101, 0x50);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0102, 0x0C);
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0053, 0x12); /* Stream ID for packets from Pipe X is 2 */
                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x005B, 0x10);

                    RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0471, 0x16); /* Concatenation register for sync mode settings (Wx4H mode) */
                }
                // -----------------------------------------------------------
                // XHS and XVS GPIO forwarding setup
                // -----------------------------------------------------------
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0003, 0x40);
                // XVS signal from SoC, DES_MFP5 --> SER_MFP7
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x02BF, 0xA3);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02D3, 0xA4);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02D4, 0x27);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02D5, 0x45);
                // XHS signal from SoC, DES_MFP0 --> SER_MFP0
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x02B0, 0xA3);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02BE, 0x24);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02BF, 0x20);

                // -----------------------------------------------------------
                // Serializer MIPI Setup
                // -----------------------------------------------------------
                // Set MIPI_RX registers number of lanes 1x4 mode
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0330, 0x00);
                Data = 0x03U | (UINT8)((pSerdesConfig->CSIRxLaneNum - 1U) << 4U); // select number of data lanes for Port B.
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0331, Data);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0332, 0xEE);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0333, 0xE4);

                // -----------------------------------------------------------
                //  IMX224 I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);

                //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
                //MFP3
                RetVal |= Max9295_9296_RegRead(VinID, SerAddr, 0x02c8, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02c8, Data);
                //MFP5
                RetVal |= Max9295_9296_RegRead(VinID, SerAddr, 0x02ce, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02ce, Data);
                //MFP6
                RetVal |= Max9295_9296_RegRead(VinID, SerAddr, 0x02d1, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SerAddr, 0x02d1, Data);
            }
            // -----------------------------------------------------------
            // Deserializer MIPI Setup
            // -----------------------------------------------------------
            // Turn off unused phys (disable phy 2/3)
            RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0332, 0x30);
            // CSI configuration
            RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0330, 0x04);
            // CSI lane mapping: 0x4E:0123, 0xB1:3210, 0x1E:10, 0xE4:23, 0xC6:03, 0x72:31, 0x8D:02
            RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0333, 0xB1);
            RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0334, 0xE4);
            // CSI number of lanes, set to 4 data lanes of controller 1
            // Please note that Bit 4 should be high, which is MPW5 (ES3.0) change
            Data = 0x10U | (UINT8)((pSerdesConfig->CSITxLaneNum[0] - 1U) << 6U); // control the number of lanes used in Port A.
            RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x044A, Data);
            // CSI bit rate per lane, CSITxSpeed * 100 Mbps
            Data = 0x20U | pSerdesConfig->CSITxSpeed[0];
            RetVal |= Max9295_9296_Remap_RegWrite(VinID, DesAddr, 0x0320, Data);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max9295_9296_Remap_SetSensorClk
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
UINT32 Max9295_9296_Remap_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkOut)
{
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    UINT8 SlaveID = 0x0U;
    const MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX9295_9296_ERR_INVALID_API;
    } else {
        if (ChipID == MX00_MAX9295_A_ID) {
            SlaveID = pCurSerDesInfo->SerAddr0;
        } else if (ChipID == MX00_MAX9295_B_ID) {
            SlaveID = pCurSerDesInfo->SerAddr1;
        } else {
            RetVal = MAX9295_9296_ERR_INVALID_API;
        }

        if (RetVal == MAX9295_9296_ERR_NONE) {
            if (RClkOut == MAX9295_9296_RCLK_OUT_FREQ_37P125M) {
                /* pre-defined clock setting is enabled */
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x0003, 0x07); /* MFP2 to output RCLKOUT */
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x0006, 0xbf);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x03f0, 0x63);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x03f0, 0x61);
            } else if (RClkOut == MAX9295_9296_RCLK_OUT_FREQ_27M) {
                /* pre-defined clock setting is enabled */
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x0003, 0x07); /* MFP2 to output RCLKOUT */
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x0006, 0xbf);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x03f0, 0x53);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x03f0, 0x51);
            } else if (RClkOut == MAX9295_9296_RCLK_OUT_FREQ_24M) {
                /* pre-defined clock setting is disabled */
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x03F1, 0x05); /* MFP2 to output RCLKOUT */
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x03F0, 0x12);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x03F4, 0x0A);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x03F5, 0x07);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x03F0, 0x10);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x1A03, 0x12);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x1A07, 0x04);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x1A08, 0x3D);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x1A09, 0x40);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x1A0A, 0xC0);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x1A0B, 0x7F);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, 0x03F0, 0x11);
            } else {
                RetVal = MAX9295_9296_ERR_ARG;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max9295_9296_Remap_SetGpioOutput
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
UINT32 Max9295_9296_Remap_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT8 Level)
{
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    UINT16 Addr = 0U; /* register address */
    MAX9295_9296_GPIO_A_REG_s GpioAReg = {0};
    MAX9295_9296_GPIO_B_REG_s GpioBReg = {0};
    const MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT8 SlaveID = 0U, RegAVal, RegBVal;

    if (ChipID == MX00_MAX9296_ID) {
        Addr = (UINT16)0x2b0 + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->DesAddr;
    } else if (ChipID == MX00_MAX9295_A_ID) {
        Addr = (UINT16)0x2be + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->SerAddr0;
    } else if (ChipID == MX00_MAX9295_B_ID) {
        Addr = (UINT16)0x2be + (UINT16)(PinID * 3U);
        SlaveID = pCurSerDesInfo->SerAddr1;
    } else {
        RetVal = MAX9295_9296_ERR_ARG;
    }
    if (RetVal == MAX9295_9296_ERR_NONE) {

        if (PinID >= AMBA_SBRG_NUM_MAX9295_9296_GPIO_PIN) {
            RetVal = MAX9295_9296_ERR_ARG;
        } else {
            RetVal |= Max9295_9296_RegRead(VinID, SlaveID, Addr, &RegAVal);
            RetVal |= Max9295_9296_RegRead(VinID, SlaveID, Addr + 1U, &RegBVal);
            if (RetVal == MAX9295_9296_ERR_NONE) {
                (void) AmbaWrap_memcpy(&GpioAReg, &RegAVal, sizeof(RegAVal));
                (void) AmbaWrap_memcpy(&GpioBReg, &RegBVal, sizeof(RegBVal));

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

                (void) AmbaWrap_memcpy(&RegAVal, &GpioAReg, sizeof(RegAVal));
                (void) AmbaWrap_memcpy(&RegBVal, &GpioBReg, sizeof(RegBVal));
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, Addr, RegAVal);
                RetVal |= Max9295_9296_Remap_RegWrite(VinID, SlaveID, Addr + 1U, RegBVal);
            } else {
                RetVal = MAX9295_9296_ERR_ARG;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max9295_9296_Remap_RegisterWrite
 *
 *  @Description:: Write Max9295_9296 registers API
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
UINT32 Max9295_9296_Remap_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 Data)
{
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    const MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX9295_9296_ERR_INVALID_API;
    } else {
        if (ChipID == MX00_MAX9296_ID) {
            RetVal = Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->DesAddr, Addr, Data);
        } else if (ChipID == MX00_MAX9295_A_ID) {
            RetVal = Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr0, Addr, Data);
        } else if (ChipID == MX00_MAX9295_B_ID) {
            RetVal = Max9295_9296_Remap_RegWrite(VinID, pCurSerDesInfo->SerAddr1, Addr, Data);
        } else {
            RetVal = MAX9295_9296_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max9295_9296_Remap_RegisterRead
 *
 *  @Description:: Read Max9295_9296 registers API
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
UINT32 Max9295_9296_Remap_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 *pData)
{
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    const MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX9295_9296_ERR_INVALID_API;
    } else {
        if (ChipID == MX00_MAX9296_ID) {
            RetVal = Max9295_9296_RegRead(VinID, pCurSerDesInfo->DesAddr, Addr, pData);
        } else if (ChipID == MX00_MAX9295_A_ID) {
            RetVal = Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr0, Addr, pData);
        } else if (ChipID == MX00_MAX9295_B_ID) {
            RetVal = Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr1, Addr, pData);
        } else {
            RetVal = MAX9295_9296_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max9295_9296_Remap_GetSerdesStatus
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
UINT32 Max9295_9296_Remap_GetSerdesStatus(UINT32 VinID, UINT16 EnabledLinkID, MAX9295_9296_SERDES_STATUS_s *pSerdesStatus)
{
    UINT8 RxData = 0x0U;
    UINT16 i, j;
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    const MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    static UINT16 VideoLockAddr[2] = {0x01fcU, 0x021cU};


    if ((EnabledLinkID & 0x3U) == 0U) {
        RetVal = ERR_ARG;
    } else {
        //By FAE, MAX9296 doesn't support link locked for each chan. LinkLock=1 if at least 1ch link locked
        //All LinkLock/VideoLock=2(NG) when unplug one or two cable after see 2ch preview
        //VideoLocked can't be checked when sensor output stops since GMSL2 mode2 does not support.
        //VideoLocked can be checked when unplug and replug any one cable
        j = 0U;

        //AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[Max9295_9296_Remap_GetSerdesStatus] EnabledLinkID %d EnabledLinkNum %d", EnabledLinkID, (UINT32)EnabledLinkNum, 0U, 0U, 0U);

        for (i=0U; i < MAX9296_NUM_RX_PORT; i ++) {

            if((EnabledLinkID & ((UINT32)1U << i)) != 0U) {

                //LinkLock
                if (Max9295_9296_RegRead(VinID, pCurSerDesInfo->DesAddr, 0x0013U, &RxData) != MAX9295_9296_ERR_NONE) {
                    pSerdesStatus[i].LinkLock = MX00_SERDES_STATUS_NOT_SUPPORTED;
                    pSerdesStatus[i].VideoLock = MX00_SERDES_STATUS_NOT_SUPPORTED;
                    RetVal |= MAX9295_9296_ERR_COMMUNICATE;
                } else {
                    if((RxData & 0x08U) == 0x08U) {
                        pSerdesStatus[i].LinkLock = MX00_SERDES_STATUS_OK;
                    } else {
                        pSerdesStatus[i].LinkLock = MX00_SERDES_STATUS_NG;
                    }
                    //VideoLock
                    if(Max9295_9296_RegRead(VinID, pCurSerDesInfo->DesAddr, VideoLockAddr[j], &RxData) != MAX9295_9296_ERR_NONE) {
                        pSerdesStatus[i].VideoLock = MX00_SERDES_STATUS_NOT_SUPPORTED;
                    } else {
                        if((RxData & 0x1U) == 0x1U) {
                            pSerdesStatus[i].VideoLock = MX00_SERDES_STATUS_OK;
                        } else {
                            pSerdesStatus[i].VideoLock = MX00_SERDES_STATUS_NG;
                        }
                    }
                }
                j ++;
            }
        }
    }

    return RetVal;

}
