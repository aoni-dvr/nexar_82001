#include <stdio.h>
#include <stdlib.h>
#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaDrvEntry.h"
#include "AmbaI2S.h"
#include "AmbaRTSL_I2S.h"
#include "AmbaCSL_I2S.h"

extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);

AMBA_I2S_REG_s *pAmbaI2S_Reg[AMBA_NUM_I2S_CHANNEL];

void TEST_AmbaI2S_DrvEntry(void)
{
    AmbaI2S_DrvEntry();

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaI2S_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    printf("TEST_AmbaI2S_DrvEntry\n");
}

void TEST_AmbaI2S_Config(void)
{
    UINT32 I2sPortIdx = AMBA_NUM_I2S_CHANNEL - 1U;
    AMBA_I2S_CTRL_s Ctrl = {0};
    AMBA_I2S_CTRL_s *pCtrl = &Ctrl;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    AmbaI2S_Config(I2sPortIdx, NULL);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaI2S_Config(I2sPortIdx, pCtrl);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    pCtrl->Mode = AMBA_I2S_MODE_DSP;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->TxCtrl.Shift = 16U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->TxCtrl.Shift = 8U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->RxCtrl.Shift = 16U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->RxCtrl.Shift = 8U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->Echo = 1U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->Mode = AMBA_I2S_MODE_I2S;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->Mode = AMBA_I2S_MODE_MSB_EXTENDED;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->ClkDirection = AMBA_I2S_MASTER;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->WordPrecision = 16U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->TxCtrl.Unison = 1U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->TxCtrl.Mute = 1U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->TxCtrl.Order = 1U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->TxCtrl.Loopback = 1U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->RxCtrl.Order = 1U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->RxCtrl.Loopback = 1U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->TxCtrl.Mono = 0x2U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    pCtrl->TxCtrl.Mono = 0x3U;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    I2sPortIdx = AMBA_NUM_I2S_CHANNEL;
    AmbaI2S_Config(I2sPortIdx, pCtrl);

    printf("TEST_AmbaI2S_Config\n");
}

void TEST_AmbaI2S_RxTrigger(void)
{
    UINT32 I2sPortIdx = AMBA_NUM_I2S_CHANNEL - 1U;
    UINT32 Cmd = 0U;
    AmbaI2S_RxTrigger(I2sPortIdx, Cmd);

    Cmd = 1U;
    AmbaI2S_RxTrigger(I2sPortIdx, Cmd);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaI2S_RxTrigger(I2sPortIdx, Cmd);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    I2sPortIdx = AMBA_NUM_I2S_CHANNEL;
    AmbaI2S_RxTrigger(I2sPortIdx, Cmd);

    printf("TEST_AmbaI2S_RxTrigger\n");
}

void TEST_AmbaI2S_TxTrigger(void)
{
    UINT32 I2sPortIdx = AMBA_NUM_I2S_CHANNEL - 1U;
    UINT32 Cmd = 0U;
    AmbaI2S_TxTrigger(I2sPortIdx, Cmd);

    Cmd = 1U;
    AmbaI2S_TxTrigger(I2sPortIdx, Cmd);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaI2S_TxTrigger(I2sPortIdx, Cmd);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    I2sPortIdx = AMBA_NUM_I2S_CHANNEL;
    AmbaI2S_TxTrigger(I2sPortIdx, Cmd);

    printf("TEST_AmbaI2S_TxTrigger\n");
}

void TEST_AmbaI2S_RxResetFifo(void)
{
    UINT32 I2sPortIdx = AMBA_NUM_I2S_CHANNEL - 1U;
    AmbaI2S_RxResetFifo(I2sPortIdx);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaI2S_RxResetFifo(I2sPortIdx);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    I2sPortIdx = AMBA_NUM_I2S_CHANNEL;
    AmbaI2S_RxResetFifo(I2sPortIdx);

    printf("TEST_AmbaI2S_RxResetFifo\n");
}

void TEST_AmbaI2S_TxResetFifo(void)
{
    UINT32 I2sPortIdx = AMBA_NUM_I2S_CHANNEL - 1U;
    AmbaI2S_TxResetFifo(I2sPortIdx);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaI2S_TxResetFifo(I2sPortIdx);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    I2sPortIdx = AMBA_NUM_I2S_CHANNEL;
    AmbaI2S_TxResetFifo(I2sPortIdx);

    printf("TEST_AmbaI2S_TxResetFifo\n");
}

void TEST_AmbaRTSL_I2sTxGetDmaAddress(void)
{
    UINT32 I2sPortIdx = AMBA_NUM_I2S_CHANNEL - 1U;
    void *TxDmaAddress = malloc(sizeof(UINT32));
    void **pTxDmaAddress = &TxDmaAddress;

    AmbaRTSL_I2sTxGetDmaAddress(I2sPortIdx, *pTxDmaAddress);

    I2sPortIdx = AMBA_NUM_I2S_CHANNEL;
    AmbaRTSL_I2sTxGetDmaAddress(I2sPortIdx, *pTxDmaAddress);

    printf("TEST_AmbaRTSL_I2sTxGetDmaAddress\n");
}

void TEST_AmbaRTSL_I2sRxGetDmaAddress(void)
{
    UINT32 I2sPortIdx = AMBA_NUM_I2S_CHANNEL - 1U;
    void *RxDmaAddress = malloc(sizeof(UINT32));
    void **pRxDmaAddress = &RxDmaAddress;

    AmbaRTSL_I2sRxGetDmaAddress(I2sPortIdx, *pRxDmaAddress);

    I2sPortIdx = AMBA_NUM_I2S_CHANNEL;
    AmbaRTSL_I2sRxGetDmaAddress(I2sPortIdx, *pRxDmaAddress);

    printf("TEST_AmbaRTSL_I2sRxGetDmaAddress\n");
}

void TEST_AmbaRTSL_I2sTxGetFifoStatus(void)
{
    UINT32 I2sPortIdx = AMBA_NUM_I2S_CHANNEL - 1U;
    UINT32 Status = 0U;
    UINT32 *pStatus = &Status;

    AmbaRTSL_I2sTxGetFifoStatus(I2sPortIdx, pStatus);

    printf("TEST_AmbaRTSL_I2sTxGetFifoStatus\n");
}

int main(void)
{
    for (UINT8 i = 0U; i < AMBA_NUM_I2S_CHANNEL; i++) {
        pAmbaI2S_Reg[i] = malloc(sizeof(AMBA_I2S_REG_s));
    }

    /* avoid AmbaRTSL_I2sInit to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaI2S_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaI2S_Config();
    TEST_AmbaI2S_RxTrigger();
    TEST_AmbaI2S_TxTrigger();
    TEST_AmbaI2S_RxResetFifo();
    TEST_AmbaI2S_TxResetFifo();

    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaRTSL_I2sTxGetDmaAddress();
    TEST_AmbaRTSL_I2sRxGetDmaAddress();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaRTSL_I2sTxGetFifoStatus();

    return 0;
}

