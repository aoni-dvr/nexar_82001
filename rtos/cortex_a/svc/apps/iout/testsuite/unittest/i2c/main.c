#include <stdio.h>
#include <stdlib.h>
#include "AmbaKAL.h"
#include "AmbaDrvEntry.h"
#include "AmbaI2C.h"
#include "AmbaRTSL_I2C.h"
#include "AmbaReg_I2C.h"
#include "AmbaCSL_I2C.h"

AMBA_I2C_REG_s * pAmbaI2C_MasterReg[AMBA_NUM_I2C_CHANNEL];
AMBA_I2CS_REG_s AmbaI2C_SlaveReg;
AMBA_I2CS_REG_s * pAmbaI2C_SlaveReg = &AmbaI2C_SlaveReg;

extern void Set_RetVal_EventFlagCreate(UINT32 RetVal);
extern void Set_RetVal_EventFlagClear(UINT32 RetVal);
extern void Set_RetVal_EventFlagGet(UINT32 RetVal);
extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);

void TEST_AmbaI2C_DrvEntry(void)
{
    AmbaI2C_DrvEntry();

    Set_RetVal_EventFlagCreate(KAL_ERR_0000);
    AmbaI2C_DrvEntry();
    Set_RetVal_EventFlagCreate(KAL_ERR_NONE);

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaI2C_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    printf("TEST_AmbaI2C_DrvEntry\n");
}

void TEST_AmbaI2C_MasterWrite(void)
{
    UINT32 MasterID = AMBA_NUM_I2C_CHANNEL - 1U;
    UINT32 BusSpeed = NUM_I2C_SPEED - 2U;
    AMBA_I2C_TRANSACTION_s TxTransaction = {0};
    AMBA_I2C_TRANSACTION_s *pTxTransaction = &TxTransaction;
    UINT32 ActualTxSize = 0U;
    UINT32 *pActualTxSize = &ActualTxSize;
    UINT32 TimeOut = 1U;
    UINT8 DataBuf = 0U;
    AmbaI2C_MasterWrite(MasterID, BusSpeed, NULL, pActualTxSize, TimeOut);

    pTxTransaction->DataSize = 1U;
    AmbaI2C_MasterWrite(MasterID, BusSpeed, pTxTransaction, pActualTxSize, TimeOut);

    pTxTransaction->pDataBuf = &DataBuf;
    AmbaI2C_MasterWrite(MasterID, BusSpeed, pTxTransaction, pActualTxSize, TimeOut);

    AmbaI2C_MasterWrite(MasterID, BusSpeed, pTxTransaction, NULL, TimeOut);

    BusSpeed = I2C_SPEED_HIGH;
    AmbaI2C_MasterWrite(MasterID, BusSpeed, pTxTransaction, pActualTxSize, TimeOut);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaI2C_MasterWrite(MasterID, BusSpeed, pTxTransaction, pActualTxSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_EventFlagGet(KAL_ERR_0000);
    AmbaI2C_MasterWrite(MasterID, BusSpeed, pTxTransaction, pActualTxSize, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_TIMEOUT);
    AmbaI2C_MasterWrite(MasterID, BusSpeed, pTxTransaction, pActualTxSize, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_NONE);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaI2C_MasterWrite(MasterID, BusSpeed, pTxTransaction, pActualTxSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaI2C_MasterWrite(MasterID, BusSpeed, pTxTransaction, pActualTxSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    pTxTransaction->DataSize = 0U;
    AmbaI2C_MasterWrite(MasterID, BusSpeed, pTxTransaction, pActualTxSize, TimeOut);

    BusSpeed = NUM_I2C_SPEED;
    AmbaI2C_MasterWrite(MasterID, BusSpeed, pTxTransaction, pActualTxSize, TimeOut);

    MasterID = AMBA_NUM_I2C_CHANNEL;
    AmbaI2C_MasterWrite(MasterID, BusSpeed, pTxTransaction, pActualTxSize, TimeOut);

    printf("TEST_AmbaI2C_MasterWrite\n");
}

void TEST_AmbaI2C_MasterRead(void)
{
    UINT32 MasterID = AMBA_NUM_I2C_CHANNEL - 1U;
    UINT32 BusSpeed = NUM_I2C_SPEED - 2U;
    AMBA_I2C_TRANSACTION_s RxTransaction = {0};
    AMBA_I2C_TRANSACTION_s *pRxTransaction = &RxTransaction;
    UINT32 ActualRxSize = 0U;
    UINT32 *pActualRxSize = &ActualRxSize;
    UINT32 TimeOut = 1U;
    UINT8 DataBuf = 0U;
    AmbaI2C_MasterRead(MasterID, BusSpeed, NULL, pActualRxSize, TimeOut);

    pRxTransaction->DataSize = 1U;
    AmbaI2C_MasterRead(MasterID, BusSpeed, pRxTransaction, pActualRxSize, TimeOut);

    pRxTransaction->pDataBuf = &DataBuf;
    AmbaI2C_MasterRead(MasterID, BusSpeed, pRxTransaction, pActualRxSize, TimeOut);

    AmbaI2C_MasterRead(MasterID, BusSpeed, pRxTransaction, NULL, TimeOut);

    BusSpeed = I2C_SPEED_HIGH;
    AmbaI2C_MasterRead(MasterID, BusSpeed, pRxTransaction, pActualRxSize, TimeOut);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaI2C_MasterRead(MasterID, BusSpeed, pRxTransaction, pActualRxSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaI2C_MasterRead(MasterID, BusSpeed, pRxTransaction, pActualRxSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaI2C_MasterRead(MasterID, BusSpeed, pRxTransaction, pActualRxSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    pRxTransaction->DataSize = 0U;
    AmbaI2C_MasterRead(MasterID, BusSpeed, pRxTransaction, pActualRxSize, TimeOut);

    BusSpeed = NUM_I2C_SPEED;
    AmbaI2C_MasterRead(MasterID, BusSpeed, pRxTransaction, pActualRxSize, TimeOut);

    MasterID = AMBA_NUM_I2C_CHANNEL;
    AmbaI2C_MasterRead(MasterID, BusSpeed, pRxTransaction, pActualRxSize, TimeOut);

    printf("TEST_AmbaI2C_MasterRead\n");
}

void TEST_AmbaI2C_MasterReadAfterWrite(void)
{
    UINT32 MasterID = AMBA_NUM_I2C_CHANNEL - 1U;
    UINT32 BusSpeed = NUM_I2C_SPEED - 2U;
    UINT32 NumTxTransaction = 1U;
    AMBA_I2C_TRANSACTION_s TxTransaction = {0};
    AMBA_I2C_TRANSACTION_s *pTxTransaction = &TxTransaction;
    AMBA_I2C_TRANSACTION_s RxTransaction = {0};
    AMBA_I2C_TRANSACTION_s *pRxTransaction = &RxTransaction;
    UINT32 ActualSize;
    UINT32 *pActualSize = &ActualSize;
    UINT32 TimeOut = 1U;
    pTxTransaction->DataSize = 1U;
    UINT8 DataBuf = 0U;
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);

    pTxTransaction->pDataBuf = &DataBuf;
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);

    pRxTransaction->DataSize = 1U;
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);

    pRxTransaction->pDataBuf = &DataBuf;
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);

    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, NULL, TimeOut);

    BusSpeed = I2C_SPEED_HIGH;
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    pRxTransaction->DataSize = 0U;
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);

    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, NULL, pActualSize, TimeOut);

    pTxTransaction->DataSize = 0U;
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);

    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, NULL, pRxTransaction, pActualSize, TimeOut);

    NumTxTransaction = 0U;
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);

    BusSpeed = NUM_I2C_SPEED;
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);

    MasterID = AMBA_NUM_I2C_CHANNEL;
    AmbaI2C_MasterReadAfterWrite(MasterID, BusSpeed, NumTxTransaction, pTxTransaction, pRxTransaction, pActualSize, TimeOut);

    printf("TEST_AmbaI2C_MasterReadAfterWrite\n");
}


void TEST_AmbaI2C_MasterReadVarLength(void)
{
    UINT32 MasterID = AMBA_NUM_I2C_CHANNEL - 1U;
    UINT32 BusSpeed = NUM_I2C_SPEED - 2U;
    UINT32 VarLenInfo = 1U;
    AMBA_I2C_TRANSACTION_s RxTransaction = {0};
    AMBA_I2C_TRANSACTION_s *pRxTransaction = &RxTransaction;
    UINT32 ActualRxSize = 0U;
    UINT32 *pActualRxSize = &ActualRxSize;
    UINT32 TimeOut = 1U;
    UINT8 DataBuf = 0U;
    AmbaI2C_MasterReadVarLength(MasterID, BusSpeed, VarLenInfo, NULL, pActualRxSize, TimeOut);

    pRxTransaction->DataSize = 1U;
    AmbaI2C_MasterReadVarLength(MasterID, BusSpeed, VarLenInfo, pRxTransaction, pActualRxSize, TimeOut);

    pRxTransaction->pDataBuf = &DataBuf;
    AmbaI2C_MasterReadVarLength(MasterID, BusSpeed, VarLenInfo, pRxTransaction, pActualRxSize, TimeOut);

    AmbaI2C_MasterReadVarLength(MasterID, BusSpeed, VarLenInfo, pRxTransaction, NULL, TimeOut);

    BusSpeed = I2C_SPEED_HIGH;
    AmbaI2C_MasterReadVarLength(MasterID, BusSpeed, VarLenInfo, pRxTransaction, pActualRxSize, TimeOut);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaI2C_MasterReadVarLength(MasterID, BusSpeed, VarLenInfo, pRxTransaction, pActualRxSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaI2C_MasterReadVarLength(MasterID, BusSpeed, VarLenInfo, pRxTransaction, pActualRxSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaI2C_MasterReadVarLength(MasterID, BusSpeed, VarLenInfo, pRxTransaction, pActualRxSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    pRxTransaction->DataSize = 0U;
    AmbaI2C_MasterReadVarLength(MasterID, BusSpeed, VarLenInfo, pRxTransaction, pActualRxSize, TimeOut);

    VarLenInfo = 0U;
    AmbaI2C_MasterReadVarLength(MasterID, BusSpeed, VarLenInfo, pRxTransaction, pActualRxSize, TimeOut);

    BusSpeed = NUM_I2C_SPEED;
    AmbaI2C_MasterReadVarLength(MasterID, BusSpeed, VarLenInfo, pRxTransaction, pActualRxSize, TimeOut);

    MasterID = AMBA_NUM_I2C_CHANNEL;
    AmbaI2C_MasterReadVarLength(MasterID, BusSpeed, VarLenInfo, pRxTransaction, pActualRxSize, TimeOut);

    printf("TEST_AmbaI2C_MasterReadVarLength\n");
}

void TEST_AmbaI2C_MasterGetInfo(void)
{
    UINT32 MasterID = AMBA_NUM_I2C_CHANNEL - 1U;
    UINT32 BusSpeed = NUM_I2C_SPEED - 2U;
    UINT32 ActualBitRate = 0U;
    UINT32 *pActualBitRate = &ActualBitRate;
    AmbaI2C_MasterGetInfo(MasterID, BusSpeed, NULL);

    AmbaI2C_MasterGetInfo(MasterID, BusSpeed, pActualBitRate);

    BusSpeed = NUM_I2C_SPEED;
    AmbaI2C_MasterGetInfo(MasterID, BusSpeed, pActualBitRate);

    MasterID = AMBA_NUM_I2C_CHANNEL;
    AmbaI2C_MasterGetInfo(MasterID, BusSpeed, pActualBitRate);

    printf("TEST_AmbaI2C_MasterGetInfo\n");
}


void TEST_AmbaI2C_SlaveStart(void)
{
    UINT32 SlaveID = AMBA_NUM_I2C_SLAVE - 1U;
    UINT32 SlaveAddr = 0x1U;
    AMBA_I2C_SLAVE_WRITE_ISR_f WriteIntFunc;
    AMBA_I2C_SLAVE_READ_ISR_f ReadIntFunc  ;
    AMBA_I2C_SLAVE_BREAK_ISR_f BreakIntFunc;
    AmbaI2C_SlaveStart(SlaveID, SlaveAddr, WriteIntFunc, ReadIntFunc, NULL);
    AmbaI2C_SlaveStart(SlaveID, SlaveAddr, WriteIntFunc, NULL, BreakIntFunc);
    AmbaI2C_SlaveStart(SlaveID, SlaveAddr, NULL, ReadIntFunc, BreakIntFunc);
    AmbaI2C_SlaveStart(SlaveID, SlaveAddr, WriteIntFunc, ReadIntFunc, BreakIntFunc);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaI2C_SlaveStart(SlaveID, SlaveAddr, WriteIntFunc, ReadIntFunc, BreakIntFunc);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaI2C_SlaveStart(SlaveID, SlaveAddr, WriteIntFunc, ReadIntFunc, BreakIntFunc);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    SlaveAddr = 0xfffU;
    AmbaI2C_SlaveStart(SlaveID, SlaveAddr, WriteIntFunc, ReadIntFunc, BreakIntFunc);

    SlaveID = AMBA_NUM_I2C_SLAVE;
    AmbaI2C_SlaveStart(SlaveID, SlaveAddr, WriteIntFunc, ReadIntFunc, BreakIntFunc);

    printf("TEST_AmbaI2C_SlaveStart\n");
}

void TEST_AmbaI2C_SlaveStop(void)
{
    UINT32 SlaveID = AMBA_NUM_I2C_SLAVE - 1U;
    AmbaI2C_SlaveStop(SlaveID);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaI2C_SlaveStop(SlaveID);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaI2C_SlaveStop(SlaveID);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    SlaveID = AMBA_NUM_I2C_SLAVE;
    AmbaI2C_SlaveStop(SlaveID);

    printf("TEST_AmbaI2C_SlaveStop\n");
}

void TEST_AmbaRTSL_I2c(void)
{
    printf("TEST_AmbaRTSL_I2c\n");
}

void TEST_AmbaRTSL_I2cWrite(void)
{
    UINT32 I2cChanNo = AMBA_NUM_I2C_CHANNEL - 1U;
    UINT32 I2cSpeed = NUM_I2C_SPEED - 2U;
    AMBA_I2C_TRANSACTION_s TxTransaction = {0};
    AMBA_I2C_TRANSACTION_s *pTxTransaction = &TxTransaction;
    UINT8 DataBuf = 0U;
    pTxTransaction->DataSize = 1U;
    AmbaRTSL_I2cWrite(I2cChanNo, I2cSpeed, pTxTransaction);

    pTxTransaction->pDataBuf = &DataBuf;
    AmbaRTSL_I2cWrite(I2cChanNo, I2cSpeed, pTxTransaction);

    I2cSpeed = NUM_I2C_SPEED;
    AmbaRTSL_I2cWrite(I2cChanNo, I2cSpeed, pTxTransaction);

    pTxTransaction->DataSize = 0U;
    AmbaRTSL_I2cWrite(I2cChanNo, I2cSpeed, pTxTransaction);

    AmbaRTSL_I2cWrite(I2cChanNo, I2cSpeed, NULL);

    I2cChanNo = AMBA_NUM_I2C_CHANNEL;
    AmbaRTSL_I2cWrite(I2cChanNo, I2cSpeed, pTxTransaction);

    printf("TEST_AmbaRTSL_I2cWrite\n");
}

void TEST_AmbaRTSL_I2cRead(void)
{
    UINT32 I2cChanNo = AMBA_NUM_I2C_CHANNEL - 1U;
    UINT32 I2cSpeed = NUM_I2C_SPEED - 2U;
    AMBA_I2C_TRANSACTION_s RxTransaction = {0};
    AMBA_I2C_TRANSACTION_s *pRxTransaction = &RxTransaction;
    UINT8 DataBuf = 0U;
    pRxTransaction->DataSize = 1U;
    AmbaRTSL_I2cRead(I2cChanNo, I2cSpeed, pRxTransaction);

    pRxTransaction->pDataBuf = &DataBuf;
    AmbaRTSL_I2cRead(I2cChanNo, I2cSpeed, pRxTransaction);

    I2cSpeed = NUM_I2C_SPEED;
    AmbaRTSL_I2cRead(I2cChanNo, I2cSpeed, pRxTransaction);

    pRxTransaction->DataSize = 0U;
    AmbaRTSL_I2cRead(I2cChanNo, I2cSpeed, pRxTransaction);

    AmbaRTSL_I2cRead(I2cChanNo, I2cSpeed, NULL);

    I2cChanNo = AMBA_NUM_I2C_CHANNEL;
    AmbaRTSL_I2cRead(I2cChanNo, I2cSpeed, pRxTransaction);

    printf("TEST_AmbaRTSL_I2cRead\n");
}

void TEST_AmbaRTSL_I2cReadAfterWrite(void)
{
    UINT32 I2cChanNo = AMBA_NUM_I2C_CHANNEL - 1U;
    UINT32 I2cSpeed = NUM_I2C_SPEED - 2U;
    UINT32 NumTxTransaction = 1U;
    AMBA_I2C_TRANSACTION_s TxTransaction = {0};
    AMBA_I2C_TRANSACTION_s *pTxTransaction = &TxTransaction;
    AMBA_I2C_TRANSACTION_s RxTransaction = {0};
    AMBA_I2C_TRANSACTION_s *pRxTransaction = &RxTransaction;
    AmbaRTSL_I2cReadAfterWrite(I2cChanNo, I2cSpeed, NumTxTransaction, pTxTransaction, pRxTransaction);

    AmbaRTSL_I2cReadAfterWrite(I2cChanNo, I2cSpeed, NumTxTransaction, pTxTransaction, NULL);
    AmbaRTSL_I2cReadAfterWrite(I2cChanNo, I2cSpeed, NumTxTransaction, NULL, pRxTransaction);

    I2cSpeed = NUM_I2C_SPEED;
    AmbaRTSL_I2cReadAfterWrite(I2cChanNo, I2cSpeed, NumTxTransaction, pTxTransaction, pRxTransaction);

    NumTxTransaction = 0U;
    AmbaRTSL_I2cReadAfterWrite(I2cChanNo, I2cSpeed, NumTxTransaction, pTxTransaction, pRxTransaction);

    I2cChanNo = AMBA_NUM_I2C_CHANNEL;
    AmbaRTSL_I2cReadAfterWrite(I2cChanNo, I2cSpeed, NumTxTransaction, pTxTransaction, pRxTransaction);

    printf("TEST_AmbaRTSL_I2cReadAfterWrite\n");
}

void TEST_AmbaRTSL_I2cReadVarLength(void)
{
    UINT32 I2cChanNo = AMBA_NUM_I2C_CHANNEL - 1U;
    UINT32 I2cSpeed = NUM_I2C_SPEED - 2U;
    UINT32 VarLenInfo = 2U;
    AMBA_I2C_TRANSACTION_s RxTransaction = {0};
    AMBA_I2C_TRANSACTION_s *pRxTransaction = &RxTransaction;
    UINT8 DataBuf = 0U;
    pRxTransaction->DataSize = 1U;
    AmbaRTSL_I2cReadVarLength(I2cChanNo, I2cSpeed, VarLenInfo, pRxTransaction);

    pRxTransaction->pDataBuf = &DataBuf;
    AmbaRTSL_I2cReadVarLength(I2cChanNo, I2cSpeed, VarLenInfo, pRxTransaction);

    I2cSpeed = NUM_I2C_SPEED;
    AmbaRTSL_I2cReadVarLength(I2cChanNo, I2cSpeed, VarLenInfo, pRxTransaction);

    pRxTransaction->DataSize = 0U;
    AmbaRTSL_I2cReadVarLength(I2cChanNo, I2cSpeed, VarLenInfo, pRxTransaction);

    AmbaRTSL_I2cReadVarLength(I2cChanNo, I2cSpeed, VarLenInfo, NULL);

    VarLenInfo = 5U;
    AmbaRTSL_I2cReadVarLength(I2cChanNo, I2cSpeed, VarLenInfo, pRxTransaction);

    VarLenInfo = 0U;
    AmbaRTSL_I2cReadVarLength(I2cChanNo, I2cSpeed, VarLenInfo, pRxTransaction);

    I2cChanNo = AMBA_NUM_I2C_CHANNEL;
    AmbaRTSL_I2cReadVarLength(I2cChanNo, I2cSpeed, VarLenInfo, pRxTransaction);

    printf("TEST_AmbaRTSL_I2cReadVarLength\n");
}

void TEST_AmbaRTSL_I2cStop(void)
{
    UINT32 I2cChanNo = AMBA_NUM_I2C_CHANNEL;
    AmbaRTSL_I2cStop(I2cChanNo);

    printf("TEST_AmbaRTSL_I2cStop\n");
}

void TEST_AmbaRTSL_I2cTerminate(void)
{
    UINT32 I2cChanNo = AMBA_NUM_I2C_CHANNEL;
    AmbaRTSL_I2cTerminate(I2cChanNo);

    printf("TEST_AmbaRTSL_I2cTerminate\n");
}

void TEST_AmbaRTSL_I2cGetResponse(void)
{
    UINT32 I2cChanNo = AMBA_NUM_I2C_CHANNEL;
    AmbaRTSL_I2cGetResponse(I2cChanNo);

    printf("TEST_AmbaRTSL_I2cGetResponse\n");
}

void TEST_AmbaRTSL_I2cGetActualSize(void)
{
    UINT32 I2cChanNo = AMBA_NUM_I2C_CHANNEL;
    AmbaRTSL_I2cGetActualSize(I2cChanNo);

    printf("TEST_AmbaRTSL_I2c\n");
}

void TEST_AmbaRTSL_I2cGetFinalState(void)
{
    UINT32 I2cChanNo = AMBA_NUM_I2C_CHANNEL - 1U;
    AmbaRTSL_I2cGetFinalState(I2cChanNo);

    I2cChanNo = AMBA_NUM_I2C_CHANNEL;
    AmbaRTSL_I2cGetFinalState(I2cChanNo);

    printf("TEST_AmbaRTSL_I2cGetFinalState\n");
}

void TEST_AmbaCSL_I2cGetPrescaler(void)
{
    AMBA_I2C_REG_s I2cReg = {0};
    AMBA_I2C_REG_s *pI2cReg = &I2cReg;
    AmbaCSL_I2cGetPrescaler(pI2cReg);

    printf("TEST_AmbaCSL_I2cGetPrescaler\n");
}

int main(void)
{
    for (UINT8 i = 0U; i < AMBA_NUM_I2C_CHANNEL; i++) {
        pAmbaI2C_MasterReg[i] = malloc(sizeof(AMBA_I2C_REG_s));
    }

    /* avoid AmbaI2C_DrvEntry to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaI2C_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaI2C_MasterWrite();
    TEST_AmbaI2C_MasterRead();
    TEST_AmbaI2C_MasterReadAfterWrite();
    TEST_AmbaI2C_MasterReadVarLength();
    TEST_AmbaI2C_MasterGetInfo();
    TEST_AmbaI2C_SlaveStart();
    TEST_AmbaI2C_SlaveStop();

    TEST_AmbaRTSL_I2c();
    TEST_AmbaRTSL_I2cWrite();
    TEST_AmbaRTSL_I2cRead();
    TEST_AmbaRTSL_I2cReadAfterWrite();
    TEST_AmbaRTSL_I2cReadVarLength();
    TEST_AmbaRTSL_I2cStop();
    TEST_AmbaRTSL_I2cTerminate();
    TEST_AmbaRTSL_I2cGetResponse();
    TEST_AmbaRTSL_I2cGetActualSize();
    TEST_AmbaRTSL_I2cGetFinalState();

    TEST_AmbaCSL_I2cGetPrescaler();

    return 0;
}

