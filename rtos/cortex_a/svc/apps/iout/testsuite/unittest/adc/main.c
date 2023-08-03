#include <stdio.h>
#include <stdlib.h>
#include "AmbaKAL.h"
#include "AmbaDrvEntry.h"
#include "AmbaADC.h"
#include "AmbaRTSL_ADC.h"
#include "AmbaCSL_ADC.h"
#include "AmbaADC_Def.h"

extern void Set_RetVal_EventFlagCreate(UINT32 RetVal);
extern void Set_RetVal_EventFlagClear(UINT32 RetVal);
extern void Set_RetVal_EventFlagGet(UINT32 RetVal);
extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);
extern void Set_RetVal_PllGetAdcClk(UINT32 RetVal);

AMBA_ADC_REG_s AmbaADC_Reg = {0};
AMBA_ADC_REG_s * pAmbaADC_Reg = &AmbaADC_Reg;
AMBA_RCT_REG_s AmbaRCT_Reg = {0};
AMBA_RCT_REG_s *const pAmbaRCT_Reg = &AmbaRCT_Reg;

void TEST_AmbaADC_DrvEntry(void)
{
    /* Avoid Floating point exception (Integer divide by zero) in AmbaRTSL_AdcConfig */
    Set_RetVal_PllGetAdcClk(AMBA_ADC_ONE_SLOT_PERIOD * AMBA_NUM_ADC_CHANNEL);

    AmbaADC_DrvEntry();

    Set_RetVal_EventFlagCreate(KAL_ERR_0000);
    AmbaADC_DrvEntry();
    Set_RetVal_EventFlagCreate(KAL_ERR_NONE);

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaADC_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    printf("TEST_AmbaADC_DrvEntry\n");
}

void TEST_AmbaADC_Config(void)
{
    UINT32 SampleRate = 0x1U;
    AmbaADC_Config(SampleRate);

    /* AmbaRTSL_ADC.c */
    SampleRate = 0x7U;
    AmbaADC_Config(SampleRate);

    SampleRate = 0x0U;
    AmbaADC_Config(SampleRate);

    printf("TEST_AmbaADC_Config\n");
}

void TEST_AmbaADC_SeamlessRead(void)
{
    UINT32 AdcCh = AMBA_NUM_ADC_CHANNEL - 1U;
    UINT32 BufSize = 0x1U;
    UINT32 Buf = 0U;
    UINT32 * pBuf = &Buf;
    UINT32 ActualSize = 0U;
    UINT32 * pActualSize = &ActualSize;
    UINT32 TimeOut = 0U;
    AmbaADC_SeamlessRead(AdcCh, BufSize, pBuf, pActualSize, TimeOut);

    AmbaADC_SeamlessRead(AdcCh, BufSize, NULL, pActualSize, TimeOut);

    Set_RetVal_EventFlagGet(KAL_ERR_0000);
    AmbaADC_SeamlessRead(AdcCh, BufSize, pBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_TIMEOUT);
    AmbaADC_SeamlessRead(AdcCh, BufSize, pBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_NONE);

    Set_RetVal_EventFlagClear(KAL_ERR_0000);
    AmbaADC_SeamlessRead(AdcCh, BufSize, pBuf, pActualSize, TimeOut);
    Set_RetVal_EventFlagClear(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaADC_SeamlessRead(AdcCh, BufSize, pBuf, pActualSize, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaADC_SeamlessRead(AdcCh, BufSize, pBuf, pActualSize, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    AmbaADC_SeamlessRead(AdcCh, BufSize, pBuf, NULL, TimeOut);

    /* AmbaRTSL_ADC.c */
    pAmbaADC_Reg->FifoStatus[AMBA_ADC_FIFO0].FifoCount = 0x2U;
    AmbaRTSL_AdcStop(pActualSize);

    AmbaRTSL_AdcStop(NULL);

    BufSize = 0x401U;
    AmbaADC_SeamlessRead(AdcCh, BufSize, pBuf, pActualSize, TimeOut);

    Set_RetVal_PllGetAdcClk(0U);
    AmbaADC_SeamlessRead(AdcCh, BufSize, pBuf, pActualSize, TimeOut);
    Set_RetVal_PllGetAdcClk(AMBA_ADC_ONE_SLOT_PERIOD * AMBA_NUM_ADC_CHANNEL);
    /* AmbaRTSL_ADC.c */

    BufSize = 0x0U;
    AmbaADC_SeamlessRead(AdcCh, BufSize, pBuf, pActualSize, TimeOut);

    AdcCh = AMBA_NUM_ADC_CHANNEL;
    AmbaADC_SeamlessRead(AdcCh, BufSize, pBuf, pActualSize, TimeOut);

    printf("TEST_AmbaADC_SeamlessRead\n");
}

void TEST_AmbaADC_SingleRead(void)
{
    UINT32 AdcCh = AMBA_NUM_ADC_CHANNEL - 1U;
    UINT32 Data = {0};
    UINT32 *pData = &Data;
    pAmbaADC_Reg->Status.Status = ADC_STATUS_COMPLETE;
    AmbaADC_SingleRead(AdcCh, pData);

    AmbaADC_SingleRead(AdcCh, NULL);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaADC_SingleRead(AdcCh, pData);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaADC_SingleRead(AdcCh, pData);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    /* AmbaRTSL_ADC.c */
    Set_RetVal_PllGetAdcClk(0U);
    AmbaADC_SingleRead(AdcCh, pData);
    Set_RetVal_PllGetAdcClk(AMBA_ADC_ONE_SLOT_PERIOD * AMBA_NUM_ADC_CHANNEL);

    AdcCh = AMBA_NUM_ADC_CHANNEL;
    AmbaADC_SingleRead(AdcCh, pData);

    printf("TEST_AmbaADC_SingleRead\n");
}

void TEST_AmbaADC_GetInfo(void)
{
    UINT32 SampleRate = 0U;
    UINT32 *pSampleRate = &SampleRate;
    UINT32 ActiveChanList = 0U;
    UINT32 *pActiveChanList = &ActiveChanList;
    AmbaADC_GetInfo(pSampleRate, pActiveChanList);

    AmbaADC_GetInfo(pSampleRate, NULL);

    AmbaADC_GetInfo(NULL, pActiveChanList);

    /* AmbaRTSL_ADC.c */
    pAmbaADC_Reg->Ctrl.Start = 0U;
    AmbaADC_GetInfo(pSampleRate, pActiveChanList);

    printf("TEST_AmbaADC_GetInfo\n");
}

int main(void)
{
    /* avoid AmbaADC_DrvEntry to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaADC_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaADC_Config();
    TEST_AmbaADC_SeamlessRead();
    TEST_AmbaADC_SingleRead();
    TEST_AmbaADC_GetInfo();

    return 0;
}


