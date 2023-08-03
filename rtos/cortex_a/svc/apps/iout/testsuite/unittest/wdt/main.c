#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDrvEntry.h"
#include "AmbaWDT.h"
#include "AmbaRTSL_WDT.h"
#include "AmbaCSL_WDT.h"
#include "AmbaReg_WDT.h"

AMBA_WDT_REG_s * pAmbaWDT_Reg[AMBA_WDT_CH_NUM];
AMBA_RCT_REG_s AmbaRCT_Reg = {0};
AMBA_RCT_REG_s *const pAmbaRCT_Reg = &AmbaRCT_Reg;

extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);

static AMBA_WDT_ISR_f ISR_Func;

void TEST_AmbaWDT_DrvEntry(void)
{
    AmbaWDT_DrvEntry();

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaWDT_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    printf("TEST_AmbaWDT_DrvEntry\n");
}

void TEST_AmbaWDT_HookTimeOutHandler(void)
{
    AMBA_WDT_ISR_f ExpireFunc = ISR_Func;
    UINT32 ExpireFuncArg = 0U;
    AmbaWDT_HookTimeOutHandler(ExpireFunc, ExpireFuncArg);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaWDT_HookTimeOutHandler(ExpireFunc, ExpireFuncArg);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaWDT_HookTimeOutHandler(ExpireFunc, ExpireFuncArg);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    ExpireFuncArg = 1U;
    AmbaWDT_HookTimeOutHandler(ExpireFunc, ExpireFuncArg);

    AMBA_WDT_ISR_f NullFunc;
    AmbaWDT_HookTimeOutHandler(NullFunc, ExpireFuncArg);

    printf("TEST_AmbaWDT_HookTimeOutHandler\n");
}

void TEST_AmbaWDT_Start(void)
{
    UINT32 CountDown = 0U;
    UINT32 ResetIrqPulseWidth = 0U;
    AmbaWDT_Start(CountDown, ResetIrqPulseWidth);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaWDT_Start(CountDown, ResetIrqPulseWidth);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaWDT_Start(CountDown, ResetIrqPulseWidth);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaWDT_Start\n");
}

void TEST_AmbaWDT_Feed(void)
{
    AmbaWDT_Feed();

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaWDT_Feed();
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaWDT_Feed();
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaWDT_Feed\n");
}

void TEST_AmbaWDT_Stop(void)
{
    AmbaWDT_Stop();

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaWDT_Stop();
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaWDT_Stop();
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaWDT_Stop\n");
}


void TEST_AmbaWDT_GetInfo(void)
{
    AMBA_WDT_INFO_s WdtInfo = {0};
    AMBA_WDT_INFO_s *pWdtInfo = &WdtInfo;
    AmbaWDT_GetInfo(pWdtInfo);

    AmbaWDT_GetInfo(NULL);

    printf("TEST_AmbaWDT_GetInfo\n");
}

void TEST_AmbaWDT_ClearStatus(void)
{
    AmbaWDT_ClearStatus();

    printf("TEST_AmbaWDT_ClearStatus\n");
}

void TEST_AmbaWDT_HookHandler(void)
{
    UINT32 ChId = 0U;
    AMBA_WDT_ISR_f ExpireFunc = ISR_Func;
    UINT32 ExpireFuncArg = 0U;
    AmbaWDT_HookHandler(ChId, ExpireFunc, ExpireFuncArg);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaWDT_HookHandler(ChId, ExpireFunc, ExpireFuncArg);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaWDT_HookHandler(ChId, ExpireFunc, ExpireFuncArg);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    ExpireFuncArg = 1U;
    AmbaWDT_HookHandler(ChId, ExpireFunc, ExpireFuncArg);

    AMBA_WDT_ISR_f NullFunc;
    AmbaWDT_HookHandler(ChId, NullFunc, ExpireFuncArg);

    printf("TEST_AmbaWDT_HookHandler\n");
}

void TEST_AmbaWDT_StartByChId(void)
{
    UINT32 ChId = 0U;
    UINT32 CountDown = 0U;
    UINT32 ResetIrqPulseWidth = 0U;
    AmbaWDT_StartByChId(ChId, CountDown, ResetIrqPulseWidth);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaWDT_StartByChId(ChId, CountDown, ResetIrqPulseWidth);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaWDT_StartByChId(ChId, CountDown, ResetIrqPulseWidth);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaWDT_StartByChId\n");
}

void TEST_AmbaWDT_FeedByChId(void)
{
    UINT32 ChId = 0U;
    AmbaWDT_FeedByChId(ChId);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaWDT_FeedByChId(ChId);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaWDT_FeedByChId(ChId);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaWDT_FeedByChId\n");
}

void TEST_AmbaWDT_StopByChId(void)
{
    UINT32 ChId = 0U;
    AmbaWDT_StopByChId(ChId);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaWDT_StopByChId(ChId);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaWDT_StopByChId(ChId);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaWDT_StopByChId\n");
}


void TEST_AmbaWDT_GetInfoByChId(void)
{
    UINT32 ChId = 0U;
    AMBA_WDT_INFO_s WdtInfo = {0};
    AMBA_WDT_INFO_s *pWdtInfo = &WdtInfo;
    AmbaWDT_GetInfoByChId(ChId, pWdtInfo);

    pAmbaRCT_Reg->WdtResetState = 0x0U;
    AmbaWDT_GetInfoByChId(ChId, pWdtInfo);

    pAmbaRCT_Reg->WdtResetState = 0x1U;
    AmbaWDT_GetInfoByChId(ChId, pWdtInfo);

    pAmbaWDT_Reg[ChId]->Ctrl = 0x2U;
    AmbaWDT_GetInfoByChId(ChId, pWdtInfo);

    pAmbaWDT_Reg[ChId]->Ctrl = 0x4U;
    AmbaWDT_GetInfoByChId(ChId, pWdtInfo);

    pAmbaWDT_Reg[ChId]->Ctrl = 0x8U;
    AmbaWDT_GetInfoByChId(ChId, pWdtInfo);

    AmbaWDT_GetInfoByChId(ChId, NULL);

    printf("TEST_AmbaWDT_GetInfoByChId\n");
}

void TEST_AmbaWDT_SetPattern(void)
{
    AmbaWDT_SetPattern();

    printf("TEST_AmbaWDT_SetPattern\n");
}

void TEST_AmbaRTSL_WdtGetStatusById(void)
{
    UINT32 ChId = 0U;
    AmbaRTSL_WdtGetStatusById(ChId);

    pAmbaRCT_Reg->WdtResetState = 0x0U;
    AmbaRTSL_WdtGetStatusById(ChId);

    pAmbaRCT_Reg->WdtResetState = 0x1U;
    AmbaRTSL_WdtGetStatusById(ChId);

    printf("TEST_AmbaRTSL_WdtGetStatusById\n");
}

void TEST_AmbaRTSL_WdtCoreFreqChg(void)
{
    UINT32 ChId = 0U;
    pAmbaWDT_Reg[ChId]->Ctrl = 0x0U;
    AmbaRTSL_WdtCoreFreqChg(ChId);

    pAmbaWDT_Reg[ChId]->Ctrl = 0x1U;
    AmbaRTSL_WdtCoreFreqChg(ChId);

    printf("TEST_AmbaRTSL_WdtCoreFreqChg\n");
}

void TEST_AmbaRTSL_WdtGetResetStatus(void)
{
    AmbaRTSL_WdtGetResetStatus();

    printf("TEST_AmbaRTSL_WdtGetResetStatus\n");
}

void TEST_AmbaCSL_WdtSetTimeOutAction(void)
{
    UINT32 TimeOutAction = WDT_ACT_NONE;
    AmbaCSL_WdtSetTimeOutAction(TimeOutAction);

    TimeOutAction = WDT_ACT_SYS_RESET;
    AmbaCSL_WdtSetTimeOutAction(TimeOutAction);

    TimeOutAction = WDT_ACT_IRQ;
    AmbaCSL_WdtSetTimeOutAction(TimeOutAction);

    TimeOutAction = WDT_ACT_EXT;
    AmbaCSL_WdtSetTimeOutAction(TimeOutAction);

    printf("TEST_AmbaCSL_WdtSetTimeOutAction\n");
}

void TEST_AmbaCSL_WdtSetAction(void)
{
    UINT32 ChId = 0U;
    UINT32 TimeOutAction = WDT_ACT_NONE;
    AmbaCSL_WdtSetAction(ChId, TimeOutAction);

    TimeOutAction = WDT_ACT_SYS_RESET;
    AmbaCSL_WdtSetAction(ChId, TimeOutAction);

    TimeOutAction = WDT_ACT_IRQ;
    AmbaCSL_WdtSetAction(ChId, TimeOutAction);

    TimeOutAction = WDT_ACT_EXT;
    AmbaCSL_WdtSetAction(ChId, TimeOutAction);

    printf("TEST_AmbaCSL_WdtSetAction\n");
}

int main(void)
{
    for (UINT8 i = 0U; i < AMBA_WDT_CH_NUM; i++) {
        pAmbaWDT_Reg[i] = malloc(sizeof(AMBA_WDT_REG_s));
    }

    /* avoid AmbaWDT_DrvEntry to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaWDT_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaWDT_HookTimeOutHandler();
    TEST_AmbaWDT_Start();
    TEST_AmbaWDT_Feed();
    TEST_AmbaWDT_Stop();
    TEST_AmbaWDT_GetInfo();
    TEST_AmbaWDT_ClearStatus();
    TEST_AmbaWDT_HookHandler();
    TEST_AmbaWDT_StartByChId();
    TEST_AmbaWDT_FeedByChId();
    TEST_AmbaWDT_StopByChId();
    TEST_AmbaWDT_GetInfoByChId();
    TEST_AmbaWDT_SetPattern();

    TEST_AmbaRTSL_WdtGetStatusById();
    TEST_AmbaRTSL_WdtCoreFreqChg();
    TEST_AmbaRTSL_WdtGetResetStatus();

    TEST_AmbaCSL_WdtSetTimeOutAction();
    TEST_AmbaCSL_WdtSetAction();

    return 0;
}

