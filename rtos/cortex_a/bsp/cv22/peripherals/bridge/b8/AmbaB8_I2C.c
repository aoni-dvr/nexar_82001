/**
 *  @file AmbaB8_I2C.c
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
 *  @details B8 I2C APIs
 *
 */
#include "AmbaB8.h"

#include "AmbaB8_Communicate.h"
#include "AmbaB8_PLL.h"
#include "AmbaB8_I2C.h"
#include "AmbaB8_GPIO.h"

#include "AmbaB8CSL_I2C.h"
#include "AmbaB8CSL_IoCtrl.h"

#define B8_I2C_CHAN_NUM         (B8_NUM_CHANNEL * B8_MAX_NUM_B8_ON_CHAN)

/*-----------------------------------------------------------------------------------------------*\
 * I2C Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32 ChipID;
    UINT32 CurrentI2c0Speed;
    UINT32 CurrentI2c1Speed;
    UINT32 CurrentI2cCtrl;
} B8_I2C_CTRL_s;

static B8_I2C_CTRL_s AmbaB8_I2cCtrl[B8_I2C_CHAN_NUM];

static AmbaB8_KAL_SEM_t AmbaB8_I2cSemID;

static UINT32 B8_I2cRegDataBuf[B8_I2C_MAX_FIFO_ENTRY];

static UINT32 B8_I2cWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                          UINT32 TxDataSize, const UINT8 *pTxDataBuf);
static UINT32 B8_I2cBurstWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                               UINT32 TxDataSize, const UINT8 *pTxDataBuf);
static UINT32 B8_I2cRead(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                         UINT32 RxDataSize, UINT8 *pRxDataBuf);
static UINT32 B8_I2cBurstRead(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                              UINT32 RxDataSize, UINT8 *pRxDataBuf);
static UINT32 B8_I2cReadAfterWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed,
                                   UINT32 TxDataSize, const UINT8 *pTxDataBuf, UINT32 RxDataSize, UINT8 *pRxDataBuf);
static UINT32 B8_I2cBurstReadAfterBurstWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed,
        UINT32 TxDataSize, const UINT8 *pTxDataBuf, UINT32 RxDataSize, UINT8 *pRxDataBuf);

static void B8_I2cWaitDataTransferComplete(UINT32 ChipID, UINT32 I2cChanNo);
static void B8_I2cWaitBurstModeDataTransferComplete(UINT32 ChipID, UINT32 I2cChanNo);
static void B8_I2cTxData(UINT32 ChipID, UINT32 I2cChanNo, UINT32 TxDataSize, const UINT8 *pTxDataBuf);
static void B8_I2cRxData(UINT32 ChipID, UINT32 I2cChanNo, UINT32 RxDataSize, UINT8 *pRxDataBuf);

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_I2cInit
 *
 *  @Description:: I2C device driver initialization
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_I2cInit(void)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT8 i;
    static UINT8 SemInitDone = 0U;

    /* I2C semaphore initialization */
    if (SemInitDone == 0U) {
        /* Create a semaphore */
        if (AmbaB8_KAL_SemCreate(&AmbaB8_I2cSemID, NULL, 1U) == OK) {
            SemInitDone = 1;
        } else {
            AmbaB8_PrintStr5("Unable to create semaphore for B8 I2cWrite", NULL, NULL, NULL, NULL, NULL);
            RetVal |= B8_ERR_MUTEX;
        }
    }

    /* Reset I2C control */
    for (i = 0; i < B8_I2C_CHAN_NUM; i++) {
        AmbaB8_I2cCtrl[i].ChipID           = 0xffffffffU;
        AmbaB8_I2cCtrl[i].CurrentI2c0Speed = 0xffffffffU;
        AmbaB8_I2cCtrl[i].CurrentI2c1Speed = 0xffffffffU;
        AmbaB8_I2cCtrl[i].CurrentI2cCtrl   = 0xffffffffU;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cSetBusSpeed
 *
 *  @Description:: Set I2C Bus Speed
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      I2cChanNo:  I2C Channel Number
 *      I2cSpeed:   I2C communication speed
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B8_I2cSetBusSpeed(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed)
{
    static B8_I2C_TIMING_s B8_I2cTiming[B8_NUM_I2C_SPEED] = {
        [B8_I2C_SPEED_STANDARD]   = {      /* For I2C Standard speed: 100Kbps */
            .Speed          = 100000,
            .DutyCycle      = 0,    /* 1:1 */
            .MinSTAHoldTime = 4000, /* ns */
        },
        [B8_I2C_SPEED_FAST]   = {          /* For I2C Fast speed: 384Kbps */
            .Speed          = 360000,
            .DutyCycle      = 0,    /* 1:1 */
            .MinSTAHoldTime = 600,  /* ns */
        },
    };

    UINT32 PrescalerRegVal;
    UINT32 Delta;
    UINT32 StretchRegVal;
    UINT32 DataBuf32[2];
    UINT32 CoreClk = AmbaB8_PllGetCoreClk(ChipID);

    AmbaB8_Misra_TouchUnused(&I2cChanNo);
    /*-----------------------------------------------------------------------*\
     * I2C Clock Frequency = (gclk_apb) / ( (4 + dutycycle[1:0]) * ( prescale[15:0] + 1 ) + delta );
     * delta = 2 + Stretch;
    \*-----------------------------------------------------------------------*/
    Delta = (CoreClk / B8_I2cTiming[I2cSpeed].Speed) % (4U + B8_I2cTiming[I2cSpeed].DutyCycle);
    if (Delta <= 2U) {
        Delta += (4U + B8_I2cTiming[I2cSpeed].DutyCycle);
    }

    PrescalerRegVal = (((CoreClk / B8_I2cTiming[I2cSpeed].Speed) - Delta) / (4U + B8_I2cTiming[I2cSpeed].DutyCycle)) - 1U;

    StretchRegVal = B8_I2cTiming[I2cSpeed].MinSTAHoldTime / ((1000000000U / CoreClk) * (PrescalerRegVal + 1U));

    DataBuf32[0] = PrescalerRegVal & 0xffU;
    DataBuf32[1] = (PrescalerRegVal >> 8U) & 0xffU;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->PrescalerLowByte), 1U, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

    DataBuf32[0] = (Delta - 2U) & 0xffU;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->StretchLowByte), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

    DataBuf32[0] = StretchRegVal & 0xffU;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->StretchCtrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

    (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_I2c0_BaseAddr->DutyCycleCtrl), 0U, B8_DATA_WIDTH_32BIT, 1, &DataBuf32[0]);
    DataBuf32[0] = ( DataBuf32[0] & 0xfffffffcU ) + B8_I2cTiming[I2cSpeed].DutyCycle;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->DutyCycleCtrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cCtrlSelect
 *
 *  @Description:: Select I2C control
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      I2cChanNo:  I2C Channel Number
 *      I2cSpeed:   I2C communication speed
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B8_I2cCtrlSelect(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed)
{
    UINT32 i;
    UINT32 DataBuf32;
    UINT32 I2CCtrlID;
    UINT32 LoopBreak = 0U;

    for (i = 0U; ((i < B8_MAX_NUM_SUBCHIP) && (LoopBreak == 0U)); i ++) {
        /* Select I2C ChipID */
        if ((ChipID & B8_SUB_CHIP_ID_MASK) == B8_SUB_CHIP_ID_B8N) { /* B8N */
            I2CCtrlID = AmbaB8_GetCtrlIndex(ChipID);
            if ((I2CCtrlID < B8_I2C_CHAN_NUM) && (AmbaB8_I2cCtrl[I2CCtrlID].ChipID == 0xffffffffU)) {
                AmbaB8_I2cCtrl[I2CCtrlID].ChipID = ChipID;
            }
            /* i = B8_MAX_NUM_SUBCHIP; *//* break the loop after */
            LoopBreak = 1U;
        } else {
            if ((ChipID & ((UINT32)1U << i)) == 0U) {               /* B8F */
                continue;
            } else {
                I2CCtrlID = AmbaB8_GetCtrlIndex((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i));
                if ((I2CCtrlID < B8_I2C_CHAN_NUM) && (AmbaB8_I2cCtrl[I2CCtrlID].ChipID == 0xffffffffU)) {
                    AmbaB8_I2cCtrl[I2CCtrlID].ChipID = ((ChipID & B8_MAIN_CHIP_ID_MASK) | (ChipID & ((UINT32)1U << i)));
                }
            }
        }

        if (I2CCtrlID < B8_I2C_CHAN_NUM) {
            /* Select I2C Speed */
            if ((I2cChanNo == B8_I2C_CHANNEL0) || (I2cChanNo == B8_I2C_CHANNEL0_2) || (I2cChanNo == B8_I2C_CHANNEL0_ALL)) {
                if (AmbaB8_I2cCtrl[I2CCtrlID].CurrentI2c0Speed != I2cSpeed) {
                    B8_I2cSetBusSpeed(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, I2cChanNo, I2cSpeed);
                    AmbaB8_I2cCtrl[I2CCtrlID].CurrentI2c0Speed = I2cSpeed;
                }
            } else {
                AmbaB8_PrintStr5("Invalid I2cChanNo!!", NULL, NULL, NULL, NULL, NULL);
            }

            /* Select I2C1 Controller */
            if (AmbaB8_I2cCtrl[I2CCtrlID].CurrentI2cCtrl != I2cChanNo) {
                if (I2cChanNo == B8_I2C_CHANNEL0) {
                    (void) AmbaB8_GpioSetAltFunc(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, B8_GPIO_PIN9_I2C0_SCL);
                    (void) AmbaB8_GpioSetAltFunc(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, B8_GPIO_PIN10_I2C0_SDA);
                    (void) AmbaB8_GpioSetAltFunc(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, B8_GPIO_PIN4);
                    (void) AmbaB8_GpioSetAltFunc(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, B8_GPIO_PIN5);

                    (void) AmbaB8_RegRead(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, & (pAmbaB8_IoCtrlReg->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
                    DataBuf32 &= 0xfffffff9U;
                    (void) AmbaB8_RegWrite(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, & (pAmbaB8_IoCtrlReg->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

                    AmbaB8_I2cCtrl[I2CCtrlID].CurrentI2cCtrl = B8_I2C_CHANNEL0;

                } else if (I2cChanNo == B8_I2C_CHANNEL0_2) {
                    (void) AmbaB8_GpioSetAltFunc(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, B8_GPIO_PIN9);
                    (void) AmbaB8_GpioSetAltFunc(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, B8_GPIO_PIN10);
                    (void) AmbaB8_GpioSetAltFunc(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, B8_GPIO_PIN4_I2C0_SCL2);
                    (void) AmbaB8_GpioSetAltFunc(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, B8_GPIO_PIN5_I2C0_SDA2);

                    (void) AmbaB8_RegRead(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, & (pAmbaB8_IoCtrlReg->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
                    DataBuf32 &= 0xfffffff9U;
                    DataBuf32 |= (UINT32)0x4U;
                    (void) AmbaB8_RegWrite(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, & (pAmbaB8_IoCtrlReg->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

                    AmbaB8_I2cCtrl[I2CCtrlID].CurrentI2cCtrl = B8_I2C_CHANNEL0_2;
                } else if (I2cChanNo == B8_I2C_CHANNEL0_ALL) {
                    (void) AmbaB8_GpioSetAltFunc(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, B8_GPIO_PIN9_I2C0_SCL);
                    (void) AmbaB8_GpioSetAltFunc(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, B8_GPIO_PIN10_I2C0_SDA);
                    (void) AmbaB8_GpioSetAltFunc(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, B8_GPIO_PIN4_I2C0_SCL2);
                    (void) AmbaB8_GpioSetAltFunc(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, B8_GPIO_PIN5_I2C0_SDA2);

                    (void) AmbaB8_RegRead(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, & (pAmbaB8_IoCtrlReg->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
                    DataBuf32 &= 0xfffffff9U;
                    (void) AmbaB8_RegWrite(AmbaB8_I2cCtrl[I2CCtrlID].ChipID, & (pAmbaB8_IoCtrlReg->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

                    AmbaB8_I2cCtrl[I2CCtrlID].CurrentI2cCtrl = B8_I2C_CHANNEL0_ALL;

                } else {
                    AmbaB8_PrintStr5("Invalid I2cChanNo!!", NULL, NULL, NULL, NULL, NULL);
                }
            }
        }
    }

    return;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_I2cWrite
 *
 *  @Description:: I2C Master write data operation
 *
 *  @Input      ::
 *      ChipID:      B8 chip id
 *      I2cChanNo:   I2C Channel Number
 *      I2cSpeed:    I2C communication speed
 *      SlaveAddr:   Slave Address
 *      TxDataSize:  Data size in Byte
 *      pTxDataBuf:  Pointer to the Tx data buffer (the 1st Byte can be the I2C Subaddress)
 *      BurstEnable: Burst mode enable flag
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_I2cWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                       UINT32 TxDataSize, const UINT8 *pTxDataBuf, UINT8 BurstEnable)
{
    UINT32 RetVal = B8_ERR_NONE;

    (void) AmbaB8_KAL_SemTake(&AmbaB8_I2cSemID, AMBAB8_KAL_WAIT_FOREVER);

    if (BurstEnable != 0U) {
        RetVal = B8_I2cBurstWrite(ChipID, I2cChanNo, I2cSpeed, SlaveAddr, TxDataSize, pTxDataBuf);
    } else {
        RetVal = B8_I2cWrite(ChipID, I2cChanNo, I2cSpeed, SlaveAddr, TxDataSize, pTxDataBuf);
    }

    (void) AmbaB8_KAL_SemGive(&AmbaB8_I2cSemID);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_I2cRead
 *
 *  @Description:: I2C Master read data operation
 *
 *  @Input      ::
 *      ChipID:      B8 chip id
 *      I2cChanNo:   I2C Channel Number
 *      I2cSpeed:    I2C communication speed
 *      SlaveAddr:   Slave Address
 *      RxDataSize:  Data size in Byte
 *      pRxDataBuf:  Pointer to the Rx data buffer
 *      BurstEnable: Burst mode enable flag
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_I2cRead(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                      UINT32 RxDataSize, UINT8 *pRxDataBuf, UINT8 BurstEnable)
{
    UINT32 RetVal = B8_ERR_NONE;

    (void) AmbaB8_KAL_SemTake(&AmbaB8_I2cSemID, AMBAB8_KAL_WAIT_FOREVER);

    if (BurstEnable != 0U) {
        RetVal = B8_I2cBurstRead(ChipID, I2cChanNo, I2cSpeed, SlaveAddr, RxDataSize, pRxDataBuf);
    } else {
        RetVal = B8_I2cRead(ChipID, I2cChanNo, I2cSpeed, SlaveAddr, RxDataSize, pRxDataBuf);
    }

    (void) AmbaB8_KAL_SemGive(&AmbaB8_I2cSemID);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_I2cReadAfterWrite
 *
 *  @Description:: I2C Master write and/or read data operation
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      I2cChanNo:  I2C Channel Number
 *      I2cSpeed:   I2C communication speed
 *      TxDataSize: TX data size
 *      pTxDataBuf: Pointer to TX data buffer
 *      RxDataSize: RX data size
 *      pRxDataBuf: Pointer to RX data buffer
 *      BurstEnable: Burst mode enable flag
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_I2cReadAfterWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed,
                                UINT32 TxDataSize, const UINT8 *pTxDataBuf, UINT32 RxDataSize, UINT8 *pRxDataBuf, UINT8 BurstEnable)
{
    UINT32 RetVal = B8_ERR_NONE;

    (void) AmbaB8_KAL_SemTake(&AmbaB8_I2cSemID, AMBAB8_KAL_WAIT_FOREVER);

    if (BurstEnable != 0U) {
        RetVal = B8_I2cBurstReadAfterBurstWrite(ChipID, I2cChanNo, I2cSpeed, TxDataSize, pTxDataBuf, RxDataSize, pRxDataBuf);
    } else {
        RetVal = B8_I2cReadAfterWrite(ChipID, I2cChanNo, I2cSpeed, TxDataSize, pTxDataBuf, RxDataSize, pRxDataBuf);
    }

    (void) AmbaB8_KAL_SemGive(&AmbaB8_I2cSemID);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cWrite
 *
 *  @Description:: I2C Master write data operation
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      I2cChanNo:  I2C Channel Number
 *      I2cSpeed:   I2C communication speed
 *      SlaveAddr:  Slave Address
 *      TxDataSize: Data size in Byte
 *      pTxDataBuf: Pointer to the Tx data buffer (the 1st Byte can be the I2C Subaddress)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_I2cWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                          UINT32 TxDataSize, const UINT8 *pTxDataBuf)
{
    UINT32 DataBuf32[1];
    UINT32 RetVal = B8_ERR_NONE;

    if ((I2cChanNo >= B8_NUM_I2C_CHANNEL) || (TxDataSize == 0U) || (pTxDataBuf == NULL)) {
        RetVal = B8_ERR_ARG;
    } else {
        /* Select I2C control */
        B8_I2cCtrlSelect(ChipID, I2cChanNo, I2cSpeed);

        /* Enable I2C */
        DataBuf32[0] = 0x01U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* I2C Slave Address for write */
        DataBuf32[0] = (UINT32)SlaveAddr & 0xfeU;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Data), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Start I2C */
        DataBuf32[0] = 0x04U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Write data to bus */
        B8_I2cTxData(ChipID, I2cChanNo, TxDataSize, pTxDataBuf);

        /* Wait previous slave address/data transfer complete */
        B8_I2cWaitDataTransferComplete(ChipID, I2cChanNo);

        /* Stop I2C */
        DataBuf32[0] = 0x08U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Disable I2C */
        DataBuf32[0] = 0x00U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
    }

    return RetVal;
}

#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cBurstWrite
 *
 *  @Description:: I2C Master write data operation
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      I2cChanNo:  I2C Channel Number
 *      I2cSpeed:   I2C communication speed
 *      SlaveAddr:  Slave Address
 *      TxDataSize: Data size in Byte
 *      pTxDataBuf: Pointer to the Tx data buffer (the 1st Byte can be the I2C Subaddress)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_I2cBurstWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                               UINT32 TxDataSize, const UINT8 *pTxDataBuf)
{
    UINT8 TxData[1];
    UINT8 FifoDataBuf[128];
    UINT32 i;
    UINT32 RetVal = B8_ERR_NONE;

    if ((I2cChanNo >= B8_NUM_I2C_CHANNEL) || (TxDataSize == 0U) || (pTxDataBuf == NULL)) {
        RetVal = B8_ERR_ARG;
    } else {
        FifoDataBuf[0] = SlaveAddr & 0xfeU;

        for (i = 0U; i < TxDataSize; i++) {
            FifoDataBuf[i + 1U] = pTxDataBuf[i];
        }

        /* Select I2C control */
        B8_I2cCtrlSelect(ChipID, I2cChanNo, I2cSpeed);

        /* Start I2C */
        TxData[0] = 0x04U;
        (void) AmbaB8_RegWrite(ChipID, &(pAmbaB8_I2c0_BaseAddr->FifoModeCtrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        /* Write data to bus */
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeWriteData), 0U, B8_DATA_WIDTH_8BIT, TxDataSize + 1U, &FifoDataBuf[0]);

        /* Stop I2C */
        TxData[0] = 0x08U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeCtrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        /* Enable I2C */
        TxData[0] = 0x01U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        /* Wait previous slave address/data transfer complete */
        B8_I2cWaitBurstModeDataTransferComplete(ChipID, I2cChanNo);

        /* Clear IF */
        TxData[0] = 0x00U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        /* Disable I2C */
        TxData[0] = 0x00U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);
    }

    return RetVal;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cBurstWrite(Supermode Write)
 *
 *  @Description:: I2C Master write data operation
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      I2cChanNo:  I2C Channel Number
 *      I2cSpeed:   I2C communication speed
 *      SlaveAddr:  Slave Address
 *      TxDataSize: Data size in Byte
 *      pTxDataBuf: Pointer to the Tx data buffer (the 1st Byte can be the I2C Subaddress)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_I2cBurstWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                               UINT32 TxDataSize, const UINT8 *pTxDataBuf)
{
    UINT8 TxData[1];
    UINT8 FifoDataBuf[128];
    UINT32 i;
    UINT32 RetVal = B8_ERR_NONE;

    if ((I2cChanNo >= B8_NUM_I2C_CHANNEL) || (TxDataSize == 0U) || (pTxDataBuf == NULL)) {
        RetVal = B8_ERR_ARG;
    } else {
        FifoDataBuf[0] = (UINT8)TxDataSize & 0xffU;   /* Packet length should be the first */
        FifoDataBuf[1] = (UINT8)SlaveAddr & 0xfeU;

        for (i = 0U; i < TxDataSize; i++) {
            FifoDataBuf[i + 2U] = pTxDataBuf[i];
        }

        /* Select I2C control */
        B8_I2cCtrlSelect(ChipID, I2cChanNo, I2cSpeed);

        /* Enable I2C (Supermode) */
        TxData[0] = 0x0dU;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        /* Write data to bus */
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeWriteData), 0U, B8_DATA_WIDTH_8BIT, TxDataSize + 2U, &FifoDataBuf[0]);

        /* Wait previous slave address/data transfer complete */
        B8_I2cWaitBurstModeDataTransferComplete(ChipID, I2cChanNo);

        /* No need to clear IF */

        /* Disable I2C */
        TxData[0] = 0x00U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cRead
 *
 *  @Description:: I2C Master read data operation
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      I2cChanNo:  I2C Channel Number
 *      I2cSpeed:   I2C communication speed
 *      SlaveAddr:  Slave Address
 *      RxDataSize: Data size in Byte
 *      pRxDataBuf: Pointer to the Rx data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_I2cRead(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                         UINT32 RxDataSize, UINT8 *pRxDataBuf)
{
    UINT32 DataBuf32[1];
    UINT32 RetVal = B8_ERR_NONE;

    if ((I2cChanNo >= B8_NUM_I2C_CHANNEL) || (RxDataSize == 0U) || (pRxDataBuf == NULL)) {
        RetVal = B8_ERR_ARG;
    } else {
        /* Select I2C control */
        B8_I2cCtrlSelect(ChipID, I2cChanNo, I2cSpeed);

        /* Enable I2C */
        DataBuf32[0] = 0x01U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* I2C Slave Address for read */
        DataBuf32[0] = (UINT32)SlaveAddr | 0x01U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Data), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Start I2C */
        DataBuf32[0] = 0x04U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Read data from bus */
        B8_I2cRxData(ChipID, I2cChanNo, RxDataSize, pRxDataBuf);

        /* Stop I2C */
        DataBuf32[0] = 0x08U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Disable I2C */
        DataBuf32[0] = 0x00U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cBurstRead
 *
 *  @Description:: I2C Master read data operation
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      I2cChanNo:  I2C Channel Number
 *      I2cSpeed:   I2C communication speed
 *      SlaveAddr:  Slave Address
 *      RxDataSize: Data size in Byte
 *      pRxDataBuf: Pointer to the Rx data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_I2cBurstRead(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed, UINT8 SlaveAddr,
                              UINT32 RxDataSize, UINT8 *pRxDataBuf)
{
    UINT8 TxData[1];
    UINT32 RetVal = B8_ERR_NONE;

    if ((I2cChanNo >= B8_NUM_I2C_CHANNEL) || (RxDataSize == 0U) || (pRxDataBuf == NULL)) {
        RetVal = B8_ERR_ARG;
    }

    /* Select I2C control */
    B8_I2cCtrlSelect(ChipID, I2cChanNo, I2cSpeed);

    /* Start I2C */
    TxData[0] = 0x04U;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeCtrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

    /* I2C Slave Address for read */
    TxData[0] = SlaveAddr | 0x01U;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeWriteData), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

    /* Set data count to read and rfifo_mode */
    TxData[0] = (UINT8)(((RxDataSize - 1U) << 5U) | 0x1U);
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeCtrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

    /* Stop I2C */
    TxData[0] = 0x08U;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeCtrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

    /* Enable I2C */
    TxData[0] = 0x01U;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

    /* Wait previous slave address/data transfer complete */
    B8_I2cWaitBurstModeDataTransferComplete(ChipID, I2cChanNo);

    /* Read data from bus */
    (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeReadData), 0U, B8_DATA_WIDTH_8BIT, RxDataSize, pRxDataBuf);

    /* Clear IF */
    TxData[0] = 0x00U;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

    /* Disable I2C */
    TxData[0] = 0x00U;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cReadAfterWrite
 *
 *  @Description:: I2C Master write and/or read data operation
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      I2cChanNo:  I2C Channel Number
 *      I2cSpeed:   I2C communication speed
 *      TxDataSize: TX data size
 *      pTxDataBuf: Pointer to TX data buffer
 *      RxDataSize: RX data size
 *      pRxDataBuf: Pointer to RX data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_I2cReadAfterWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed,
                                   UINT32 TxDataSize, const UINT8 *pTxDataBuf, UINT32 RxDataSize, UINT8 *pRxDataBuf)
{
    UINT32 DataBuf32[1];
    UINT32 RetVal = B8_ERR_NONE;

    if ((I2cChanNo >= B8_NUM_I2C_CHANNEL) || (TxDataSize == 0U) || (RxDataSize == 0U) || (pTxDataBuf == NULL) || (pRxDataBuf == NULL)) {
        RetVal = B8_ERR_ARG;
    } else {
        /* Select I2C control */
        B8_I2cCtrlSelect(ChipID, I2cChanNo, I2cSpeed);

        /* Enable I2C */
        DataBuf32[0] = 0x01U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* I2C Slave Address */
        DataBuf32[0] = (UINT32)pTxDataBuf[0] & 0xfeU;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Data), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Start I2C */
        DataBuf32[0] = 0x04U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Write data to bus */
        B8_I2cTxData(ChipID, I2cChanNo, TxDataSize - 1U, &pTxDataBuf[1]);

        /* Wait previous slave address/data transfer complete */
        B8_I2cWaitDataTransferComplete(ChipID, I2cChanNo);

        /* I2C Slave Address for read */
        DataBuf32[0] = (UINT32)pTxDataBuf[0] | 0x01U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Data), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Start I2C */
        DataBuf32[0] = 0x04U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Read data from bus */
        B8_I2cRxData(ChipID, I2cChanNo, RxDataSize, pRxDataBuf);

        /* Stop I2C */
        DataBuf32[0] = 0x08U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Disable I2C */
        DataBuf32[0] = 0x00U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cBurstReadAfterBurstWrite
 *
 *  @Description:: I2C Master write and/or read data operation
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      I2cChanNo:  I2C Channel Number
 *      I2cSpeed:   I2C communication speed
 *      TxDataSize: TX data size
 *      pTxDataBuf: Pointer to TX data buffer
 *      RxDataSize: RX data size
 *      pRxDataBuf: Pointer to RX data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_I2cBurstReadAfterBurstWrite(UINT32 ChipID, UINT32 I2cChanNo, UINT32 I2cSpeed,
        UINT32 TxDataSize, const UINT8 *pTxDataBuf, UINT32 RxDataSize, UINT8 *pRxDataBuf)
{
    UINT8 TxData[1];
    UINT32 Quotient, Remainder, i;
    UINT32 RetVal = B8_ERR_NONE;
    UINT8 *pRxDataBuf_ptr;

    if ((I2cChanNo >= B8_NUM_I2C_CHANNEL) || (TxDataSize == 0U) || (RxDataSize == 0U) || (pTxDataBuf == NULL) || (pRxDataBuf == NULL)) {
        RetVal = B8_ERR_ARG;
    } else {
        pRxDataBuf_ptr = pRxDataBuf;
        Quotient = RxDataSize / B8_I2C_MAX_RX_FIFO_ENTRY;
        Remainder = RxDataSize % B8_I2C_MAX_RX_FIFO_ENTRY;

        /* Select I2C control */
        B8_I2cCtrlSelect(ChipID, I2cChanNo, I2cSpeed);

        /* Start I2C */
        TxData[0] = 0x04U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeCtrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        /* I2C Slave Address */
        TxData[0] = pTxDataBuf[0] & 0xfeU;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeWriteData), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        /* Write data to bus */
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeWriteData), 0U, B8_DATA_WIDTH_8BIT, TxDataSize - 1U, &pTxDataBuf[1]);

        /* Start I2C */
        TxData[0] = 0x04U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeCtrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        /* I2C Slave Address for read */
        TxData[0] = pTxDataBuf[0] | 0x01U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeWriteData), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        /* Set data count to read and rfifo_mode */
        if (Quotient == 0U) {
            TxData[0] = (UINT8)(((RxDataSize - 1U) << 5U) | 0x1U);
        } else {
            TxData[0] = (UINT8)((((UINT8)B8_I2C_MAX_RX_FIFO_ENTRY - 1U) << 5U) | 0x1U);
        }
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeCtrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        /* Stop I2C */
        TxData[0] = 0x08U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeCtrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        /* Enable I2C */
        TxData[0] = 0x01U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        /* Wait previous slave address/data transfer complete */
        B8_I2cWaitBurstModeDataTransferComplete(ChipID, I2cChanNo);

        if (Quotient == 0U) {
            /* Read data from bus */
            (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeReadData), 0U, B8_DATA_WIDTH_8BIT, RxDataSize, pRxDataBuf_ptr);

            /* Clear IF */
            TxData[0] = 0x00U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

            /* Disable I2C */
            TxData[0] = 0x00U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

        } else {
            /* Read data from bus */
            (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_I2c0_BaseAddr->FifoModeReadData), 0U, B8_DATA_WIDTH_8BIT, B8_I2C_MAX_RX_FIFO_ENTRY, pRxDataBuf_ptr);
            pRxDataBuf_ptr = &pRxDataBuf_ptr[B8_I2C_MAX_RX_FIFO_ENTRY];

            /* Clear IF */
            TxData[0] = 0x00U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

            /* Disable I2C */
            TxData[0] = 0x00U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Enable), 0U, B8_DATA_WIDTH_8BIT, 1, TxData);

            /* Read remaining data by B8_I2cBurstRead() */
            for (i = 0U; i < (Quotient - 1U); i++) {
                RetVal |= B8_I2cBurstRead(ChipID, I2cChanNo, I2cSpeed, (pTxDataBuf[0] | 0x01U), B8_I2C_MAX_RX_FIFO_ENTRY, pRxDataBuf_ptr);
                pRxDataBuf_ptr = &pRxDataBuf_ptr[B8_I2C_MAX_RX_FIFO_ENTRY];
            }

            RetVal |= B8_I2cBurstRead(ChipID, I2cChanNo, I2cSpeed, (pTxDataBuf[0] | 0x01U), Remainder, pRxDataBuf_ptr);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cWaitDataTransferComplete
 *
 *  @Description:: Wait for slave address/data transfer complete
 *
 *  @Input      ::
 *      ChipID:    B8 chip id
 *      I2cChanNo: I2C Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B8_I2cWaitDataTransferComplete(UINT32 ChipID, UINT32 I2cChanNo)
{
    UINT32 DataBuf32[4][1] = {0};
    UINT32 *DataBuf[4] = {DataBuf32[0], DataBuf32[1], DataBuf32[2], DataBuf32[3]};
    void* pRxDataBuf[4] = {NULL};
    UINT8 CompleteFlag = 0;//, i;
    UINT8 TimeOut = 200;

    AmbaB8_Misra_TouchUnused(&I2cChanNo);
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[0], &DataBuf[0], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[1], &DataBuf[1], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[2], &DataBuf[2], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[3], &DataBuf[3], sizeof(void*));

    (void) AmbaB8_Wrap_memset(B8_I2cRegDataBuf, 0x0, sizeof(B8_I2cRegDataBuf));
#if 0
    if (AmbaB8_GetSubChipCount(ChipID & B8_SUB_CHIP_ID_MASK) > 1U) {
        do {
            CompleteFlag = 1;

            (void) AmbaB8_RegMultiRead(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, pRxDataBuf);
            TimeOut --;
            for (i = 0; i < B8_MAX_NUM_B8F_ON_CHAN; i ++) {
                if (((ChipID & ((UINT32)1U << i)) != 0U) && ((DataBuf[i][0] & 0x2U) == 0U)) {
                    CompleteFlag = 0;
                }
            }
        } while ((TimeOut != 1U) && (CompleteFlag != 1U));
    } else
#endif
    {
        do {
            (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf[0]);
            TimeOut --;
            if ((DataBuf[0][0] & 0x2U) != 0U) {
                CompleteFlag = 1;
            }
        } while ((TimeOut != 1U) && (CompleteFlag != 1U));
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cWaitBurstModeDataTransferComplete
 *
 *  @Description:: Wait for slave address/data transfer complete
 *
 *  @Input      ::
 *      ChipID:    B8 chip id
 *      I2cChanNo: I2C Channel Number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B8_I2cWaitBurstModeDataTransferComplete(UINT32 ChipID, UINT32 I2cChanNo)
{
    UINT32 DataBuf32[4][1] = {0};
    UINT32 *DataBuf[4] = {DataBuf32[0], DataBuf32[1], DataBuf32[2], DataBuf32[3]};
    void* pRxDataBuf[4] = {NULL};
    UINT8 CompleteFlag = 0;//, i;
    UINT8 TimeOut = 200;

    AmbaB8_Misra_TouchUnused(&I2cChanNo);
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[0], &DataBuf[0], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[1], &DataBuf[1], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[2], &DataBuf[2], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[3], &DataBuf[3], sizeof(void*));
#if 0
    if (AmbaB8_GetSubChipCount(ChipID & B8_SUB_CHIP_ID_MASK) > 1U) { /* broadcast mode */
        do {
            CompleteFlag = 1;

            (void) AmbaB8_RegMultiRead(ChipID, & (pAmbaB8_I2c0_BaseAddr->Status), 0U, B8_DATA_WIDTH_32BIT, 1, pRxDataBuf);
            TimeOut --;
            for (i = 0; i < B8_MAX_NUM_B8F_ON_CHAN; i ++) {
                if (((ChipID & ((UINT32)1U << i)) != 0U) && ((DataBuf[i][0] & 0xf0U) != 0U)) {
                    CompleteFlag = 0;
                }
            }
        } while ((TimeOut != 1U) && (CompleteFlag != 1U));
    } else
#endif
    {
        do {
            (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_I2c0_BaseAddr->Status), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf[0]);
            TimeOut --;
            if ((DataBuf[0][0] & 0xf0U) == 0U) {
                CompleteFlag = 1;
            }
        } while ((TimeOut != 1U) && (CompleteFlag != 1U));
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cTxData
 *
 *  @Description:: Write data to I2C bus
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      TxDataSize: sending data size in Frames
 *      pTxDataBuf: pointer to the sending data buffer
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B8_I2cTxData(UINT32 ChipID, UINT32 I2cChanNo, UINT32 TxDataSize, const UINT8 *pTxDataBuf)
{
    UINT32 DataBuf32[1];
    UINT32 i;

    for (i = 0U; i < TxDataSize; i++) {
        /* Wait previous slave address/data transfer complete */
        B8_I2cWaitDataTransferComplete(ChipID, I2cChanNo);

        /* Prepare data for the next data transfer */
        DataBuf32[0] = pTxDataBuf[i];
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Data), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Clear complete flag and start the next data transfer */
        DataBuf32[0] = 0x00U;
        (void) AmbaB8_RegWrite(ChipID, &(pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_I2cRxData
 *
 *  @Description:: Read data from I2C bus
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      RxDataSize: sending data size in Frames
 *      pRxDataBuf: pointer to the sending data buffer
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void B8_I2cRxData(UINT32 ChipID, UINT32 I2cChanNo, UINT32 RxDataSize, UINT8 *pRxDataBuf)
{
    UINT32 *pRegDataBuf = B8_I2cRegDataBuf;
    UINT32 DataBuf32[1];
    UINT32 i;

    /* Wait previous slave address transfer complete */
    B8_I2cWaitDataTransferComplete(ChipID, I2cChanNo);

    for (i = 0U; i < (RxDataSize - 1U); i++) {
        /* Clear complete flag and start the next data transfer */
        DataBuf32[0] = 0x00U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Wait previous data transfer complete */
        B8_I2cWaitDataTransferComplete(ChipID, I2cChanNo);

        /* Read data from data port */
        (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_I2c0_BaseAddr->Data), 0U, B8_DATA_WIDTH_32BIT, 1, pRegDataBuf);
        pRxDataBuf[i] = (UINT8)pRegDataBuf[0];
    }

    /* Clear complete flag and start the next data transfer */
    DataBuf32[0] = 0x01U;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_I2c0_BaseAddr->Ctrl), 0U, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

    /* Wait previous data transfer complete */
    B8_I2cWaitDataTransferComplete(ChipID, I2cChanNo);

    /* Read data from data port */
    (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_I2c0_BaseAddr->Data), 0U, B8_DATA_WIDTH_32BIT, 1, pRegDataBuf);
    pRxDataBuf[i] = (UINT8)pRegDataBuf[0];
}
