#include <stdio.h>
#include <stdlib.h>
#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaDrvEntry.h"
#include "AmbaGDMA.h"
#include "AmbaRTSL_GDMA.h"
#include "AmbaCSL_GDMA.h"

extern void Set_RetVal_TimerCreate(UINT32 RetVal);
extern void Set_RetVal_EventFlagCreate(UINT32 RetVal);
extern void Set_RetVal_EventFlagGet(UINT32 RetVal);
extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);

AMBA_GDMA_REG_s AmbaGDMA_Reg = {0};
AMBA_GDMA_REG_s *pAmbaGDMA_Reg = &AmbaGDMA_Reg;

void TEST_AmbaGDMA_DrvEntry(void)
{
    AmbaGDMA_DrvEntry();

    Set_RetVal_EventFlagCreate(KAL_ERR_0000);
    AmbaGDMA_DrvEntry();
    Set_RetVal_EventFlagCreate(KAL_ERR_NONE);

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaGDMA_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    Set_RetVal_TimerCreate(KAL_ERR_0000);
    AmbaGDMA_DrvEntry();
    Set_RetVal_TimerCreate(KAL_ERR_NONE);

    printf("TEST_AmbaGDMA_DrvEntry\n");
}


void TEST_AmbaGDMA_GetInfo(void)
{
    UINT32 NumAvails = 0U;
    UINT32 * pNumAvails = &NumAvails;
    AmbaGDMA_GetInfo(pNumAvails);

    AmbaGDMA_GetInfo(NULL);

    printf("TEST_AmbaGDMA_GetInfo\n");
}

void TEST_AmbaGDMA_WaitAllCompletion(void)
{
    UINT32 TimeOut = 0x1U;
    // RTSL_GdmaGetNumAvails = 4U;
    // timer_change = 1U;
    AmbaGDMA_WaitAllCompletion(TimeOut);

    // timer_change = 0U;
    AmbaGDMA_WaitAllCompletion(TimeOut);

    // RTSL_GdmaGetNumAvails = GDMA_NUM_INSTANCE;
    Set_RetVal_EventFlagGet(KAL_ERR_TIMEOUT);
    AmbaGDMA_WaitAllCompletion(TimeOut);

    TimeOut = KAL_WAIT_NEVER;
    AmbaGDMA_WaitAllCompletion(TimeOut);

    // RTSL_GdmaGetNumAvails = 0U;
    AmbaGDMA_WaitAllCompletion(TimeOut);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGDMA_WaitAllCompletion(TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    // RTSL_GdmaGetNumAvails = GDMA_NUM_INSTANCE;
    AmbaGDMA_WaitAllCompletion(TimeOut);

    // RTSL_GdmaGetNumAvails = 0U;

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGDMA_WaitAllCompletion(TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaGDMA_WaitAllCompletion\n");
}

void TEST_AmbaGDMA_LinearCopy(void)
{
    AMBA_GDMA_LINEAR_s LinearBlit = {0};
    AMBA_GDMA_LINEAR_s * pLinearBlit = &LinearBlit;
    AMBA_GDMA_ISR_f NotifierFunc;
    UINT32 NotifierFuncArg = 0U;
    UINT32 TimeOut = 0U;
    AmbaGDMA_LinearCopy(pLinearBlit, NotifierFunc, NotifierFuncArg, TimeOut);

    AmbaGDMA_LinearCopy(NULL, NotifierFunc, NotifierFuncArg, TimeOut);

    pLinearBlit->NumPixels = GDMA_MAX_WIDTH + 1U;
    AmbaGDMA_LinearCopy(pLinearBlit, NotifierFunc, NotifierFuncArg, TimeOut);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGDMA_LinearCopy(pLinearBlit, NotifierFunc, NotifierFuncArg, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGDMA_LinearCopy(pLinearBlit, NotifierFunc, NotifierFuncArg, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaGDMA_LinearCopy\n");
}

void TEST_AmbaGDMA_BlockCopy(void)
{
    AMBA_GDMA_BLOCK_s BlockBlit = {0};
    AMBA_GDMA_BLOCK_s * pBlockBlit = &BlockBlit;
    AMBA_GDMA_ISR_f NotifierFunc;
    UINT32 NotifierFuncArg = 0U;
    UINT32 TimeOut = 0U;
    AmbaGDMA_BlockCopy(pBlockBlit, NotifierFunc, NotifierFuncArg, TimeOut);

    AmbaGDMA_BlockCopy(NULL, NotifierFunc, NotifierFuncArg, TimeOut);

    // RTSL_GdmaGetNumAvails = 1U;
    AmbaGDMA_BlockCopy(pBlockBlit, NotifierFunc, NotifierFuncArg, TimeOut);
    // RTSL_GdmaGetNumAvails = 0U;

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGDMA_BlockCopy(pBlockBlit, NotifierFunc, NotifierFuncArg, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGDMA_BlockCopy(pBlockBlit, NotifierFunc, NotifierFuncArg, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaGDMA_BlockCopy\n");
}


void TEST_AmbaGDMA_ColorKeying(void)
{
    AMBA_GDMA_BLOCK_s BlockBlit = {0};
    AMBA_GDMA_BLOCK_s * pBlockBlit = &BlockBlit;
    AMBA_GDMA_ISR_f NotifierFunc;
    UINT32 TransparentColor = 0U;
    UINT32 NotifierFuncArg = 0U;
    UINT32 TimeOut = 0U;
    AmbaGDMA_ColorKeying(pBlockBlit, TransparentColor, NotifierFunc, NotifierFuncArg, TimeOut);

    AmbaGDMA_ColorKeying(NULL, TransparentColor, NotifierFunc, NotifierFuncArg, TimeOut);

    // RTSL_GdmaGetNumAvails = 1U;
    AmbaGDMA_ColorKeying(pBlockBlit, TransparentColor, NotifierFunc, NotifierFuncArg, TimeOut);
    // RTSL_GdmaGetNumAvails = 0U;

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGDMA_ColorKeying(pBlockBlit, TransparentColor, NotifierFunc, NotifierFuncArg, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGDMA_ColorKeying(pBlockBlit, TransparentColor, NotifierFunc, NotifierFuncArg, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaGDMA_ColorKeying\n");
}


void TEST_AmbaGDMA_AlphaBlending(void)
{
    AMBA_GDMA_BLOCK_s BlockBlit = {0};
    AMBA_GDMA_BLOCK_s * pBlockBlit = &BlockBlit;
    AMBA_GDMA_ISR_f NotifierFunc;
    UINT32 AlphaVal = 0U;
    UINT32 BlendMode = 0U;
    UINT32 NotifierFuncArg = 0U;
    UINT32 TimeOut = 0U;
    AmbaGDMA_AlphaBlending(pBlockBlit, AlphaVal, BlendMode, NotifierFunc, NotifierFuncArg, TimeOut);

    AmbaGDMA_AlphaBlending(NULL, AlphaVal, BlendMode, NotifierFunc, NotifierFuncArg, TimeOut);

    // RTSL_GdmaGetNumAvails = 1U;
    AmbaGDMA_AlphaBlending(pBlockBlit, AlphaVal, BlendMode, NotifierFunc, NotifierFuncArg, TimeOut);
    // RTSL_GdmaGetNumAvails = 0U;

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGDMA_AlphaBlending(pBlockBlit, AlphaVal, BlendMode, NotifierFunc, NotifierFuncArg, TimeOut);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGDMA_AlphaBlending(pBlockBlit, AlphaVal, BlendMode, NotifierFunc, NotifierFuncArg, TimeOut);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaGDMA_AlphaBlending\n");
}


int main(void)
{

    /* avoid AmbaGDMA_DrvEntry to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaGDMA_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaGDMA_GetInfo();
    TEST_AmbaGDMA_WaitAllCompletion();
    TEST_AmbaGDMA_LinearCopy();
    TEST_AmbaGDMA_BlockCopy();
    TEST_AmbaGDMA_ColorKeying();
    TEST_AmbaGDMA_AlphaBlending();

    return 0;
}

