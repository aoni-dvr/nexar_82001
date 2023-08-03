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
#include "AmbaUtility.h"

#include "AmbaI2C.h"
#include "AmbaVIN.h"
#include "AmbaSbrg_Max9295_9296.h"

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
 *  @RoutineName:: Max9295_9296_RegWrite
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
static UINT32 Max9295_9296_RegWrite(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 Data)
{
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[3];
    UINT32 TxSize;

    I2cConfig.SlaveAddr = SlaveID;
    I2cConfig.DataSize  = 3U;
    I2cConfig.pDataBuf  = TxDataBuf;

    TxDataBuf[0] = (UINT8) (Addr >> 8U);
    TxDataBuf[1] = (UINT8) (Addr & 0xffU);
    TxDataBuf[2] = Data;

    RetVal = AmbaI2C_MasterWrite(MAX9295_9296I2cChannel[VinID], AMBA_I2C_SPEED_FAST, &I2cConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "VinID%d I2C does not work!!!!!, RetVal=0x%x", VinID, RetVal, 0U, 0U, 0U);
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
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
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

    RetVal = AmbaI2C_MasterReadAfterWrite(MAX9295_9296I2cChannel[VinID], AMBA_I2C_SPEED_FAST, 1U,
                                          &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "VinID %d I2CChan %d SlaveID 0x%02x Addr 0x%04x I2C does not work!!!!!", VinID, MAX9295_9296I2cChannel[VinID], SlaveID, Addr, 0U);
    }

    return RetVal;
}

static UINT32 Max9295_9296_RegReadModifyWrite(UINT32 VinID, UINT8 SlaveID, UINT16 Addr, UINT8 Data, UINT8 Mask)
{
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    UINT8 RData;

    RetVal = Max9295_9296_RegRead(VinID, SlaveID, Addr, &RData);

    if (RetVal == MAX9295_9296_ERR_NONE) {
        RetVal = Max9295_9296_RegWrite(VinID, SlaveID, Addr, Data | (RData & ~Mask));
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

        (void) AmbaKAL_TaskSleep(1);
        Count++;
    }

    if (Count == TimeOut) {
        AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Max9296 Reg0x13=0x%02x (%d) NG - GMSL2 NOT locked!", RxData, Count, 0U, 0U, 0U);
        RetVal = MAX9295_9296_ERR_SERDES_LINK;
    } else {
        AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Max9296 Reg0x13=0x%02x (%d) OK - GMSL2 locked!", RxData, Count, 0U, 0U, 0U);
    }

    return RetVal;
}

UINT32 Max9295_9296_DeInit(UINT32 VinID)
{
    MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    pCurSerDesInfo->InitDone = 0;

    return MAX9295_9296_ERR_NONE;
}

static UINT32 Max9295_9296_InitSer(UINT32 VinID, UINT16 EnabledLinkID)
{
    MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    UINT8 Data;


    static const UINT8 IdcAddrMax9296[AMBA_NUM_VIN_CHANNEL] = {
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

    static const UINT8 IdcAddrMax9295[AMBA_NUM_VIN_CHANNEL][2] = {
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

    pCurSerDesInfo->DesAddr = IdcAddrMax9296[VinID];
    pCurSerDesInfo->SerAddr0 = IdcAddrMax9295[VinID][0];
    pCurSerDesInfo->SerAddr1 = IdcAddrMax9295[VinID][1];

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
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x03, 0x3U);
                //set MFP2 as GPIO low
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x2c4, 0x80U);
                //MAX9295 reset all
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x10, 0x81U); //TBD: 0x81 for LinkA only by FAE
            }
            if(Rval1 == MAX9295_9296_ERR_NONE) {
                ///set MFP2 as GPIO low to make CFG1 works fine when reset MAX9295
                //disable MAX9295 clk_si
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x03, 0x3U);
                //set MFP2 as GPIO low
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x2c4, 0x80U);
                //MAX9295 reset all
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x10, 0x81U); //TBD: 0x81 for LinkA only by FAE
            }

            //Check locked
            (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
            (void) Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200);
        } else {
            AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] not reset MAX9295 for 1st bootup case", NULL, NULL, NULL, NULL, NULL);
        }

        AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Please skip \"i2c doesn't work\" msg if not do reboot without power off/on Max9296.", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Max9295_9296_Init should return OK for this case", NULL, NULL, NULL, NULL, NULL);
        if (EnabledLinkID == 0x1U) {
            AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] One-shot reset for Link A", NULL, NULL, NULL, NULL, NULL);
            RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x21U);
            //wait 25ms then check locked
            (void)AmbaKAL_TaskSleep(25U);
            if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200U) != MAX9295_9296_ERR_NONE) {
                RetVal = MAX9295_9296_ERR_SERDES_LINK;
            } else {
                if(Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0000U, &Data) != MAX9295_9296_ERR_NONE) {
                    RetVal |= Max9295_9296_RegWrite(VinID, MX00_IDC_ADDR_MAX9295_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr0);
                } else {
                    //LinkA MAX9295 i2c transmission done
                }
            }
        } else if (EnabledLinkID == 0x2U) {
            AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] One-shot reset for Link B", NULL, NULL, NULL, NULL, NULL);
            RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x22U);
            //wait 25ms then check locked
            (void)AmbaKAL_TaskSleep(25U);
            if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200U) != MAX9295_9296_ERR_NONE) {
                RetVal = MAX9295_9296_ERR_SERDES_LINK;
            } else {
                if(Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0000U, &Data) != MAX9295_9296_ERR_NONE) {
                    RetVal |= Max9295_9296_RegWrite(VinID, MX00_IDC_ADDR_MAX9295_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr1);
                } else {
                    //LinkB MAX9295 i2c transmission done
                }
            }
        } else { /* if (EnabledLinkID == 0x3) */
            AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] One-shot reset for Link A", NULL, NULL, NULL, NULL, NULL);
            RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x21U);
            //wait 25ms then check locked
            (void)AmbaKAL_TaskSleep(25U);
            if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200U) != MAX9295_9296_ERR_NONE) {
                RetVal = MAX9295_9296_ERR_SERDES_LINK;
            } else {
                if(Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr0, 0x0000U, &Data) != MAX9295_9296_ERR_NONE) {
                    RetVal |= Max9295_9296_RegWrite(VinID, MX00_IDC_ADDR_MAX9295_DEFAULT, 0x0000U, pCurSerDesInfo->SerAddr0);
                } else {
                    //LinkA MAX9295 i2c transmission done
                }
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x006BU, 0x16U);
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x0073U, 0x17U);
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x007BU, 0x36U);
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x0083U, 0x36U);
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x0093U, 0x36U);
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x009BU, 0x36U);
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x00A3U, 0x36U);
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x00ABU, 0x36U);
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x008BU, 0x36U);

                AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] One-shot reset for Link B", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x22U);
                //Check locked
                (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
                if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200) != MAX9295_9296_ERR_NONE) {
                    RetVal = MAX9295_9296_ERR_SERDES_LINK;
                } else {
                    if(Max9295_9296_RegRead(VinID, pCurSerDesInfo->SerAddr1, 0x0000U, &Data) != MAX9295_9296_ERR_NONE) {
                        RetVal |= Max9295_9296_RegWrite(VinID, MX00_IDC_ADDR_MAX9295_DEFAULT, 0x0000, pCurSerDesInfo->SerAddr1);
                    } else {
                        //LinkB MAX9295 i2c transmission done
                    }
                }
                AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] one-shot reset and deserializer splitter mode", NULL, NULL, NULL, NULL, NULL);
                RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010U, 0x23U);
                //Check locked
                (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
                if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200) != MAX9295_9296_ERR_NONE) {
                    RetVal = MAX9295_9296_ERR_SERDES_LINK;
                }
            }
        }
    }

    return RetVal;
}

UINT32 Max9295_9296_Init(UINT32 VinID, UINT16 EnabledLinkID)
{
    MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    static UINT8 Init = 0;
    static AMBA_KAL_MUTEX_t AmbaMX00Mutex;
    UINT8 Data, Data1, ChkVer, i;
    char charBuf[5];
    static const UINT8 IdcAddrMax9296[AMBA_NUM_VIN_CHANNEL] = {
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

    static const UINT8 IdcAddrMax9295[AMBA_NUM_VIN_CHANNEL][2] = {
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

    const MAX9295_9296_CHIP_VERSION_s ChipVerAry[8] = {
        {0x00U, 1.0f},
        {0x01U, 1.1f},
        {0x02U, 2.0f},
        {0xFFU, 2.1f},
        {0x04U, 3.0f},
        {0x05U, 4.0f},
        {0x06U, 5.0f},
        {0x07U, 99.0f}, //Production Samples

    };

    if (pCurSerDesInfo->InitDone == 0U) {
        AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Init", NULL, NULL, NULL, NULL, NULL);
    } else {
        AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Init for Vin lost Recovery", NULL, NULL, NULL, NULL, NULL);
    }

    /* Create Mutex */
    if (AmbaKAL_MutexCreate(&AmbaMX00Mutex, NULL) == OK) {
        AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Create Mutex OK", 0U, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Mutex had been created", 0U, 0U, 0U, 0U, 0U);
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
            if (Data == 0x94U) {
                ChkVer = 0U;
                for (i = 0U; i < 8U; i ++) {
                    if ( Data1 == ChipVerAry[i].Reg0xE ) {
                        ChkVer = 1;
                        if (ChipVerAry[i].Ver == 99.0f) {
                            AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Supported Production Samples MAX9296(new), apply PHYA/PHB optimization", 0U, 0U, 0U, 0U, 0U);
                            //PHYA/PHB optimization before one shot reset for Productions Sample by FAE's suggestion(19/06)
                            RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1458, 0x28);
                            RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1459, 0x68);

                            RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1558, 0x28);
                            RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x1559, 0x68);
                        } else {
                            (void) AmbaUtility_FloatToStr(&charBuf[0],  5U, ChipVerAry[i].Ver, 1U);
                            AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Supported ES%s MAX9296", charBuf, NULL, NULL, NULL, NULL);
                        }
                        break;
                    }
                }
                if (ChkVer == 0U) {
                    AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Not in MAX9296 Supported List", 0U, 0U, 0U, 0U, 0U);
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

    /*
     * Take the Mutex
     */
    if (AmbaKAL_MutexTake(&AmbaMX00Mutex, 100) == OK) {
        //Need to check vin recovery for multi MAX9296
        if (Init == 0U) {
            //turn off all Vin0/Vin1 MAX9295 to avoid conflict i2c slave addr
            AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] turn off all Vin0/Vin1 MAX9295 to avoid conflict i2c slave addr", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintStr5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Please skip \"I2C does not work\" msg if not connect to those MAX9296 i2c SlaveID", NULL, NULL, NULL, NULL, NULL);

            for (i = 0U; i < AMBA_NUM_VIN_CHANNEL; i ++) {
                if(Max9295_9296_RegRead(i, IdcAddrMax9296[i], 1, &Data) == MAX9295_9296_ERR_NONE) {
                    Data |= (UINT8)0x10; // set DIS_REM_CC for Vin0/Vin2 MAX9296
                    RetVal |= Max9295_9296_RegWrite(i, IdcAddrMax9296[i], 1, Data);
                } else {
                    //No vin0/vin2 MAX9296
                }
            }

            Init = 1U;
        }
        /*
         * Release the Mutex
         */
        (void)AmbaKAL_MutexGive(&AmbaMX00Mutex);

    }  else {
        AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Take Mutex NG", 0U, 0U, 0U, 0U, 0U);
        RetVal |= MAX9295_9296_ERR_MUTEX;
    }
    AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Turn on Vin%d Max9295 slave addr", VinID, 0U, 0U, 0U, 0U);
    //turn on Config vin MAX9295
    RetVal |= Max9295_9296_RegRead(VinID, pCurSerDesInfo->DesAddr, 1, &Data);
    Data &= (UINT8)0xEF; // clear DIS_REM_CC
    RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->DesAddr, 1, Data);

    RetVal |= Max9295_9296_InitSer(VinID, EnabledLinkID);

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
    Max9295_9296_RegWrite(VinID, pCurSerDesInfo->DesAddr, 0x0010, 0x31);

    if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 100) != OK)
        return NG;
#endif

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max9295_9296_Config
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
UINT32 Max9295_9296_Config(UINT32 VinID, const MAX9295_9296_SERDES_CONFIG_s *pSerdesConfig)
{
    const MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    UINT16 EnabledLinkID;
    UINT8 SensorSlaveID, SensorAliasID;
    UINT8 SerAddr, DesAddr;
    UINT8 Data;
    UINT8 HighBandwidthModeEn;
    UINT8 Index = 0U; //array index for NumDataBits, 0/1/2/3: 10/12/14/16 bit
    const UINT8 Reg0x0313A[4] = {0x01U, 0x10U, 0x00U, 0x00U}; //1chA/B and 2chA
    const UINT8 Reg0x0313B[4] = {0x04U, 0x40U, 0x00U, 0x00U}; //2chB
    const UINT8 DataType[4] = {0x2BU, 0x2CU, 0x2DU, 0x2EU}; //RAW10/12/14/16
    const UINT8 Reg0x031C[4] = {0x34U, 0x38U, 0x00U, 0x00U}; //Reg0x031E
    const UINT8 Reg0x0473[4] = {0x04U, 0x01U, 0x00U, 0x00U};

    if (pSerdesConfig == NULL) {
        RetVal = MAX9295_9296_ERR_ARG;
    } else {
        if (pCurSerDesInfo->InitDone == 0U) {
            RetVal = MAX9295_9296_ERR_INVALID_API;
        } else {
            EnabledLinkID       = pSerdesConfig->EnabledLinkID;
            SensorSlaveID       = pSerdesConfig->SensorSlaveID;
            HighBandwidthModeEn = pSerdesConfig->HighBandwidthModeEn;
            DesAddr             = pCurSerDesInfo->DesAddr;
            Index               = (pSerdesConfig->NumDataBits - 10U) / 2U;

            AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[MAX9295_9296] Config (EnabledLinkID: 0x%02x, HighBandwidthModeEn: %d, NumDataBits: %d, Index:%d)...", EnabledLinkID, HighBandwidthModeEn, pSerdesConfig->NumDataBits, Index, 0U);

            if ((EnabledLinkID & 0x1U) != 0U) {
                /*---------------------------------------------
                 * [LINK A only or in reserve splitter mode]
                 * SER: MIPI Port B -> Pipe X (TX Stream ID=1)
                 * DES: Pipe Y (RX Stream ID=1) -> Controller 1
                 ----------------------------------------------*/
                SerAddr = pCurSerDesInfo->SerAddr0;
                SensorAliasID = pSerdesConfig->SensorAliasID[0];

                if (HighBandwidthModeEn == 0U) {
                    // -----------------------------------------------------------
                    // Video Pipes setup
                    // -----------------------------------------------------------
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0312, 0x01);
                    Data = 0x20U | (pSerdesConfig->NumDataBits & 0x1fU);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031C, Data);
                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0473, 0x10);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0102, 0x0E);
                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0112, 0x23); /* Packet detector enabled by FAE to check video locked when reset sensor. Roll back since preview NG*/
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0308, 0x71); /* Video Pipe X from Port B */
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0311, 0x10);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0100, 0x60); //Use BPP from BPP register
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0101, 0x50);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0102, 0x0C);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0053, 0x11); /* Stream ID for packets from Pipe X is 1 */
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0057, 0x10);
                } else {
                    if (EnabledLinkID == 3U) {
                        //[TBD] reset LinkA/B one shot reset
                        RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, 0x0010U, 0x21U);
                        RetVal |= Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr1, 0x0010U, 0x21U);
                        //Check locked
                        (void)AmbaKAL_TaskSleep(25); //delay 25ms then check locked (LOCK takes at least 25ms after PWDN)
                        if (Max9295_9296_WaitLockStatus(VinID, pCurSerDesInfo->DesAddr, 200) != MAX9295_9296_ERR_NONE) {
                            RetVal = MAX9295_9296_ERR_SERDES_LINK;
                        }
                     }
                 }
                // -----------------------------------------------------------
                // XHS and XVS GPIO forwarding setup
                // -----------------------------------------------------------
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0003, 0x40);
                // XVS signal from SoC, DES_MFP5 --> SER_MFP7
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x02BF, 0xA3);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02D3, 0xA4);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02D4, 0x27);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02D5, 0x45);
                // XHS signal from SoC, DES_MFP0 --> SER_MFP0
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x02B0, 0xA3);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02BE, 0x24);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02BF, 0x20);

                // -----------------------------------------------------------
                // Serializer MIPI Setup
                // -----------------------------------------------------------
                if (HighBandwidthModeEn == 1U) {
                    // Disable heart beat and disable drift detect for pipe X/Y/Z/U
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0102, 0x0C);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x010A, 0x0C);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0112, 0x0C);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x011A, 0x0C);

                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0334, 0x00); // Port polarity 2 Port polarity 3
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0335, 0x00); // Port polarity 0 Port polarity 1 Port polarity clock
                    if (EnabledLinkID == 1U) {
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0311, 0x15); // Start Port Pipe X Start Port Pipe Y 0x15
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0308, 0x7F); // Pipe X clock selected Pipe Y clock selected Pipe
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0002, 0xF3); // Pipe X video transmission enable Pipe Y video transmission enable Pipe Z video transmission enable Pipe U video transmission enable
                    } else { //EnabledLinkID == 3U
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0311, 0x55); // Start Port Pipe X Start Port Pipe Y, A: 0x55, B:0x45(0x55 OK), 2ch
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0308, 0x73); // Pipe X clock selected Pipe Y clock selected Pipe Z clock selected Pipe U clock selected
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0002, 0x33); // Pipe X video transmission enable Pipe Y video transmission enable
                    }
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0053, 0x10); // Pipe X stream select 0
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0057, 0x11); // Pipe Y stream select 1
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x005B, 0x12); // Pipe Z stream select 2
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x005F, 0x13); // Pipe U stream select 3
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0100, 0x68); //Use BPP from mipi receiver
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0110, 0x68);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0101, 0x58);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0111, 0x58);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0312, 0x00); // BPP 8 Double

                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0313, Reg0x0313A[Index]); // BPP 10/12 Double
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0314, (0x40U | DataType[Index])); // Memory Datatype 1 Pipe X

                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0315, 0x00); // Memory Datatype 2 Pipe X
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0316, 0x00); // Memory Datatype 1 Pipe Y (0x37)
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0317, 0x00); // Memory Datatype 2 Pipe Y
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0318, 0x00); // Memory Datatype 1 Pipe Z
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0319, 0x00); // Memory Datatype 2 Pipe Z
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031A, 0x00); // Memory Datatype 1 Pipe U
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031B, 0x00); // Memory Datatype 2 Pipe U
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031C, Reg0x031C[Index]); // Soft BPP Pipe X

                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031D, 0x00); // Soft BPP Pipe Y
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031E, 0x00); // Soft BPP Pipe Z
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031F, 0x00); // Soft BPP Pipe U
                }
                // Set MIPI_RX registers number of lanes 1x4 mode
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0330, 0x00);
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum - 1U) << 4U)); // select number of data lanes for Port B.
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0331, Data);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0332, 0xEE);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0333, 0xE4);

                // -----------------------------------------------------------
                //  IMX224 I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);

                //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
                //MFP3
                RetVal |= Max9295_9296_RegRead(VinID, SerAddr, 0x02c8, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02c8, Data);
                //MFP5
                RetVal |= Max9295_9296_RegRead(VinID, SerAddr, 0x02ce, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02ce, Data);
                //MFP6
                RetVal |= Max9295_9296_RegRead(VinID, SerAddr, 0x02d1, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02d1, Data);
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
                    if (HighBandwidthModeEn == 0U) {
                        // -----------------------------------------------------------
                        // Video Pipes setup
                        // -----------------------------------------------------------
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0312, 0x01);
                        Data = 0x20U | (pSerdesConfig->NumDataBits & 0x1fU);
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031C, Data);
                        RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0473, 0x10);
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0102, 0x0E);
                        RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0112, 0x23); /* Packet detector enabled by FAE to check video locked when reset sensor. Roll back since preview NG*/
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0308, 0x71); /* Video Pipe X from Port B */
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0311, 0x10);
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0100, 0x60); //Use BPP from BPP register
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0101, 0x50);
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0102, 0x0C);
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0053, 0x11); /* Stream ID for packets from Pipe X is 1 */
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0057, 0x10);
                    }
                } else { /* if (EnabledLinkID== 0x3) */
                    /*---------------------------------------------
                     * [LINK B in reverse splitter mode]
                     * SER: MIPI Port B -> Pipe X (TX Stream ID=2)
                     * DES: Pipe Z (RX Stream ID=2) -> Controller 2
                     ----------------------------------------------*/
                    if (HighBandwidthModeEn == 0U) {
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0312, 0x01);
                        Data = 0x20U | (pSerdesConfig->NumDataBits & 0x1fU);
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031C, Data);
                        RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04B3, 0x10);
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0102, 0x0E);
                        RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0124, 0x23); /* Packet detector enabled by FAE to check video locked when reset sensor. Roll back since preview NG*/
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0308, 0x71); /* Video Pipe X from Port B */
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0311, 0x10);
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0100, 0x60); //Use BPP from BPP register
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0101, 0x50);
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0102, 0x0C);
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0053, 0x12); /* Stream ID for packets from Pipe X is 2 */
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x005B, 0x10);
                        //Wx2H
                        RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0471, 0x16); /* Concatenation register for sync mode settings (Wx4H mode) */
                    }
                }
                // -----------------------------------------------------------
                // XHS and XVS GPIO forwarding setup
                // -----------------------------------------------------------
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0003, 0x40);
                // XVS signal from SoC, DES_MFP5 --> SER_MFP7
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x02BF, 0xA3);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02D3, 0xA4);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02D4, 0x27);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02D5, 0x45);
                // XHS signal from SoC, DES_MFP0 --> SER_MFP0
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x02B0, 0xA3);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02BE, 0x24);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02BF, 0x20);

                // -----------------------------------------------------------
                // Serializer MIPI Setup
                // -----------------------------------------------------------
                if (HighBandwidthModeEn == 1U) {
                    // Disable heart beat and disable drift detect for pipe X/Y/Z/U
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0102, 0x0C);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x010A, 0x0C);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0112, 0x0C);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x011A, 0x0C);

                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0334, 0x00); // Port polarity 2 Port polarity 3
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0335, 0x00); // Port polarity 0 Port polarity 1 Port polarity clock
                    if (EnabledLinkID == 2U) {
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0311, 0x15); // Start Port Pipe X Start Port Pipe Y 0x15
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0308, 0x7F); // Pipe X clock selected Pipe Y clock selected Pipe
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0002, 0xF3); // Pipe X video transmission enable Pipe Y video transmission enable Pipe Z video transmission enable Pipe U video transmission enable

                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0313, Reg0x0313A[Index]); // BPP 10 Double BPP 12 Double
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0314, (0x40U | DataType[Index])); // Memory Datatype 1 Pipe X

                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0318, 0x00); // Memory Datatype 1 Pipe Z

                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031C, Reg0x031C[Index]); // Soft BPP Pipe X

                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031E, 0x00); // Soft BPP Pipe Z
                    } else { //EnabledLinkID == 3U
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0311, 0x45); //
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0308, 0x7C); // Pipe Z clock selected Pipe U clock selected
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0002, 0xC3); // Pipe Z video transmission enable Pipe U video transmission enable

                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0313, Reg0x0313B[Index]); // BPP 10 Double
                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0314, 0x00); // Memory Datatype 1 Pipe X

                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0318, (0x40U | DataType[Index])); // Memory Datatype 1 Pipe Z

                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031C, 0x00); // Soft BPP Pipe X

                        RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031E, Reg0x031C[Index]); // Soft BPP Pipe Z, A:0x00, B:0x34
                    }
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0053, 0x10); // Pipe X stream select 0
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0057, 0x11); // Pipe Y stream select 1
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x005B, 0x12); // Pipe Z stream select 2
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x005F, 0x13); // Pipe U stream select 3
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0100, 0x68); //Use BPP from mipi receiver
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0110, 0x68);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0101, 0x58);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0111, 0x58);
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0312, 0x00); // BPP 8 Double

                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0315, 0x00); // Memory Datatype 2 Pipe X
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0316, 0x00); // Memory Datatype 1 Pipe Y (0x37)
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0317, 0x00); // Memory Datatype 2 Pipe Y

                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0319, 0x00); // Memory Datatype 2 Pipe Z
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031A, 0x00); // Memory Datatype 1 Pipe U
                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031B, 0x00); // Memory Datatype 2 Pipe U

                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031D, 0x00); // Soft BPP Pipe Y

                    RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x031F, 0x00); // Soft BPP Pipe U
                }
                // Set MIPI_RX registers number of lanes 1x4 mode
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0330, 0x00);
                Data = (UINT8)(0x03U | ((pSerdesConfig->CSIRxLaneNum - 1U) << 4U)); // select number of data lanes for Port B.
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0331, Data);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0332, 0xEE);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0333, 0xE4);

                // -----------------------------------------------------------
                //  IMX224 I2C tanslation
                // -----------------------------------------------------------
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0042, SensorAliasID);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x0043, SensorSlaveID);

                //enable MAX9295 MFP3/5/6 pulldown resistor to meet description in datasheet
                //MFP3
                RetVal |= Max9295_9296_RegRead(VinID, SerAddr, 0x02c8, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02c8, Data);
                //MFP5
                RetVal |= Max9295_9296_RegRead(VinID, SerAddr, 0x02ce, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02ce, Data);
                //MFP6
                RetVal |= Max9295_9296_RegRead(VinID, SerAddr, 0x02d1, &Data);
                Data = (Data | 0x80U);
                RetVal |= Max9295_9296_RegWrite(VinID, SerAddr, 0x02d1, Data);
            }
            // -----------------------------------------------------------
            // Deserializer MIPI Setup
            // -----------------------------------------------------------

            if (HighBandwidthModeEn == 0U) {
                // Turn off unused phys (disable phy 2/3)
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0332, 0x30);
                // CSI configuration
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0330, 0x04);

                // CSI number of lanes, set to 4 data lanes of controller 1
                // Please note that Bit 4 should be high, which is MPW5 (ES3.0) change
                Data = 0x10U | (UINT8)((pSerdesConfig->CSITxLaneNum[0] - 1U) << 6U); // control the number of lanes used in Port A.
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x044A, Data);
            } else {
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0100, 0x23); // Seq_miss_en fixed to 0 for CSI Dis_pkt_det fixed to 1 for CSI
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0112, 0x23); // Seq_miss_en fixed to 0 for CSI Dis_pkt_det fixed to 1 for CSI
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0124, 0x23); // Seq_miss_en fixed to 0 for CSI Dis_pkt_det fixed to 1 for CSI
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0136, 0x23); // Seq_miss_en fixed to 0 for CSI Dis_pkt_det fixed to 1 for CSI

                // CSI configuration
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0330, 0x04);
                // Turn off unused phys (disable phy 2/3)
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0332, 0xF0);

                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x040A, 0x10);
                // CSI number of lanes, set to 4 data lanes of controller 1
                // Please note that Bit 4 should be high, which is MPW5 (ES3.0) change
                Data = 0x10U | (UINT8)((pSerdesConfig->CSITxLaneNum[0] - 1U) << 6U); // control the number of lanes used in Port A.
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x044A, Data);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x048A, Data);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04CA, 0x10);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0326, 0x2F);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0050, 0x00); // Pipe X stream select 0
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0051, 0x01); // Pipe Y stream select 1
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0052, 0x02); // Pipe Z stream select 2
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0053, 0x03); // Pipe U stream select 3
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0335, 0x00); // Port A polarity 0 Port A polarity 1 Port A polarity 2 Port A polarity 3 Port A polarity clock
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0336, 0x00); // Port B polarity 0 Port B polarity 1 Port B polarity 2 Port B polarity 3 Port B polarity clock
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0339, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x033A, 0x02);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x033B, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x033C, 0x02);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0433, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0473, Reg0x0473[Index]); // Un-double data CTRL1
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04B3, 0x00);

                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04F3, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x040B, 0x07);  //A: 0x07, B:0x00 2ch

                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x040C, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x044B, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x044C, 0x00);
                if (EnabledLinkID == 3U) {
                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x048B, 0x07); // Enable mappings for Pipe Z, A: 0x00, B:0x07

                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04AD, 0x15); // Destination for Pipe Z, A:0x00, B:0x15

                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x048D, DataType[Index]); // Pipe Z DT and VC Source, [TBD] A:0x00, B:0x2B
                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x048E, DataType[Index]); // Pipe Z DT and VC Destination A:0x00, B:0x2B

                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0491, 0x01); // Pipe Z Frame End Source A:0x00, B:0x01
                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0492, 0x01); // Pipe Z Frame End Destination A:0x00, B:0x01

                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x030D, 0x01); // Disabled VC VSYNC Check for pipe Z (VC 0-7) //A:0x00, B:0x01
                } else { //1ch
                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x048B, 0x00); // Enable mappings for Pipe Z

                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04AD, 0x00); // Destination for Pipe Z

                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x048D, 0x00); // Pipe Z DT and VC Source, [TBD] Output bit
                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x048E, 0x00); // Pipe Z DT and VC Destination

                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0491, 0x00); // Pipe Z Frame End Source
                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0492, 0x00); // Pipe Z Frame End Destination

                    RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x030D, 0x00); // Disabled VC VSYNC Check for pipe Z (VC 0-7)
                }
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x048C, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04CB, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04CC, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x042D, 0x15);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x042E, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x042F, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0430, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x046D, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x046E, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x046F, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0470, 0x00);

                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04AE, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04AF, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04B0, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04ED, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04EE, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04EF, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04F0, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x040D, DataType[Index]);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x040E, DataType[Index]);

                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x040F, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0410, 0x00);

                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0411, 0x01);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0412, 0x01);

                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x044D, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x044E, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x044F, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0450, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0451, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0452, 0x00);

                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x048F, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0490, 0x00);

                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04CD, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04CE, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04CF, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04D0, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04D1, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04D2, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0309, 0x01);

                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x030B, 0x00);

                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x030F, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0413, 0x00); //[TBD] RAW12
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0414, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0415, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0416, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0417, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0418, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0419, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x041A, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x041B, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x041C, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x041D, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x041E, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x041F, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0420, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0421, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0422, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0423, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0424, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0425, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0426, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0427, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0428, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0429, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x042A, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x042B, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x042C, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0453, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0454, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0455, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0456, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0457, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0458, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0459, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x045A, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x045B, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x045C, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x045D, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x045E, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x045F, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0460, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0461, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0462, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0463, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0464, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0465, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0466, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0467, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0468, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0469, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x046A, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x046B, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x046C, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0493, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0494, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0495, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0496, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0497, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0498, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0499, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x049A, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x049B, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x049C, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x049D, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x049E, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x049F, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04A0, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04A1, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04A2, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04A3, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04A4, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04A5, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04A6, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04A7, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04A8, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04A9, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04AA, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04AB, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04AC, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04D3, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04D4, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04D5, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04D6, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04D7, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04D8, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04D9, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04DA, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04DB, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04DC, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04DD, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04DE, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04DF, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04E0, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04E1, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04E2, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04E3, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04E4, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04E5, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04E6, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04E7, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04E8, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04E9, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04EA, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04EB, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x04EC, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0313, 0x02);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0314, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0315, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0316, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0317, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x031A, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0318, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0319, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x031D, 0x2F);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x031C, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x031F, 0x00);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x01D9, 0x19);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x01F9, 0x19);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0219, 0x19);
                RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0239, 0x19);
            }
            // CSI lane mapping
            RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0333, 0x4E);
            RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0334, 0xE4);

            // CSI bit rate per lane, CSITxSpeed * 100 Mbps
            Data = 0x20U | pSerdesConfig->CSITxSpeed[0];
            RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0320, Data);
            RetVal |= Max9295_9296_RegWrite(VinID, DesAddr, 0x0323, Data);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max9295_9296_SetSensorClk
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
UINT32 Max9295_9296_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkOut)
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
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x0003, 0x07); /* MFP2 to output RCLKOUT */
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x0006, 0xbf);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x03f0, 0x63);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x03f0, 0x61);
            } else if (RClkOut == MAX9295_9296_RCLK_OUT_FREQ_27M) {
                // settings generated by GMSL SerDes GUI V5.4.0 DPLL Tool, Output RCLK_ALT (MFP #2)
                // # f_in = 25 MHz.  # f_out = 27 MHz.  # f_out - f_out_real = -30 Hz.
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x02, 0x43);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x03F4, 0xEC);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x03F5, 0x01, 0x0F);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x00, 0x03);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x1A03, 0x10, 0x90);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x1A07, 0x04, 0xFC);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x1A08, 0x45);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x1A09, 0x40, 0xFF);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x1A0A, 0x40, 0x7F);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x1A0B, 0x60, 0xE0);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x01, 0x01);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x0003, 0x07, 0x07);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x0006, 0x20, 0x20);
            } else if (RClkOut == MAX9295_9296_RCLK_OUT_FREQ_24M) {
                /* pre-defined clock setting is disabled */
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x03F1, 0x05); /* MFP2 to output RCLKOUT */
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x03F0, 0x12);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x03F4, 0x0A);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x03F5, 0x07);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x03F0, 0x10);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x1A03, 0x12);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x1A07, 0x04);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x1A08, 0x3D);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x1A09, 0x40);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x1A0A, 0xC0);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x1A0B, 0x7F);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x03F0, 0x11);
            } else if (RClkOut == MAX9295_9296_RCLK_OUT_FREQ_26973027) {
                // settings generated by GMSL SerDes GUI V5.4.0 DPLL Tool, Output RCLK_ALT (MFP #2)
                // # f_in = 25 MHz.  # f_out = 26.973027 MHz.  # f_out - f_out_real = -30 Hz.
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x02, 0x43);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x03F4, 0xD1);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x03F5, 0x00, 0x0F);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x00, 0x03);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x1A03, 0x10, 0x90);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x1A07, 0x04, 0xFC);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, 0x1A08, 0x45);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x1A09, 0x40, 0xFF);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x1A0A, 0x40, 0x7F);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x1A0B, 0x60, 0xE0);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x03F0, 0x01, 0x01);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x0003, 0x07, 0x07);
                RetVal |= Max9295_9296_RegReadModifyWrite(VinID, SlaveID, 0x0006, 0x20, 0x20);
            } else {
                RetVal = MAX9295_9296_ERR_ARG;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max9295_9296_SetGpioOutput
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
UINT32 Max9295_9296_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT8 Level)
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
                if (AmbaWrap_memcpy(&GpioAReg, &RegAVal, sizeof(RegAVal)) != ERR_NONE) {
                    RetVal = MAX9295_9296_ERR_UNEXPECTED;
                }
                if (AmbaWrap_memcpy(&GpioBReg, &RegBVal, sizeof(RegBVal)) != ERR_NONE) {
                    RetVal = MAX9295_9296_ERR_UNEXPECTED;
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
                    RetVal = MAX9295_9296_ERR_UNEXPECTED;
                }
                if (AmbaWrap_memcpy(&RegBVal, &GpioBReg, sizeof(RegBVal)) != ERR_NONE) {
                    RetVal = MAX9295_9296_ERR_UNEXPECTED;
                }
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, Addr, RegAVal);
                RetVal |= Max9295_9296_RegWrite(VinID, SlaveID, Addr + 1U, RegBVal);
            } else {
                RetVal = MAX9295_9296_ERR_ARG;
            }
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max9295_9296_RegisterWrite
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
UINT32 Max9295_9296_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 Data)
{
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    const MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];

    if (pCurSerDesInfo->InitDone == 0U) {
        RetVal = MAX9295_9296_ERR_INVALID_API;
    } else {
        if (ChipID == MX00_MAX9296_ID) {
            RetVal = Max9295_9296_RegWrite(VinID, pCurSerDesInfo->DesAddr, Addr, Data);
        } else if (ChipID == MX00_MAX9295_A_ID) {
            RetVal = Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr0, Addr, Data);
        } else if (ChipID == MX00_MAX9295_B_ID) {
            RetVal = Max9295_9296_RegWrite(VinID, pCurSerDesInfo->SerAddr1, Addr, Data);
        } else {
            RetVal = MAX9295_9296_ERR_ARG;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaSbrg_Max9295_9296_RegisterRead
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
UINT32 Max9295_9296_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 *pData)
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
 *  @RoutineName:: Max9295_9296_GetSerdesStatus
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
UINT32 Max9295_9296_GetSerdesStatus(UINT32 VinID, UINT16 EnabledLinkID, MAX9295_9296_SERDES_STATUS_s *pSerdesStatus)
{
    UINT8 RxData = 0x0U;
    UINT16 i, j;
    UINT32 RetVal = MAX9295_9296_ERR_NONE;
    const MAX9295_9296_SERDES_INFO_s *pCurSerDesInfo = &MaxSerDesInfo[VinID];
    static const UINT16 VideoLockAddr[2] = {0x01fcU, 0x021cU};


    if ((EnabledLinkID & 0x3U) == 0U) {
        RetVal = ERR_ARG;
    } else {
        //By FAE, MAX9296 doesn't support link locked for each chan. LinkLock=1 if at least 1ch link locked
        //All LinkLock/VideoLock=2(NG) when unplug one or two cable after see 2ch preview
        //VideoLocked can't be checked when sensor output stops since GMSL2 mode2 does not support.
        //VideoLocked can be checked when unplug and replug any one cable
        j = 0U;

        //AmbaPrint_ModulePrintUInt5(MAX9295_9296_MODULE_ID, "[Max9295_9296_GetSerdesStatus] EnabledLinkID %d EnabledLinkNum %d", EnabledLinkID, (UINT32)EnabledLinkNum, 0U, 0U, 0U);

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
