#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDrvEntry.h"
#include "AmbaTMR.h"
#include "AmbaRTSL_TMR.h"
#include "AmbaCSL_TMR.h"
#include "AmbaReg_TMR.h"

#define AMBA_NUM_AVAIL_TIMER AMBA_R52_TIMER_OFFSET

AMBA_TMR_REG_s AmbaTMR_Regs[2] = {0};
AMBA_TMR_REG_s *pTempAddr0 = &AmbaTMR_Regs[0];
AMBA_TMR_REG_s *pTempAddr1 = &AmbaTMR_Regs[1];
AMBA_TMR_REG_s *const pAmbaTMR_Regs[2];


extern void Set_RetVal_EventFlagCreate(UINT32 RetVal);
extern void Set_RetVal_EventFlagGet(UINT32 RetVal);
extern void Set_RetVal_EventFlagSet(UINT32 RetVal);
extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);
extern void Set_RetVal_PllGetApbClk(UINT32 RetVal);

void TEST_AmbaTMR_DrvEntry(void)
{
    AmbaTMR_DrvEntry();

    Set_RetVal_EventFlagSet(KAL_ERR_0000);
    AmbaTMR_DrvEntry();
    Set_RetVal_EventFlagSet(KAL_ERR_NONE);

    Set_RetVal_EventFlagCreate(KAL_ERR_0000);
    AmbaTMR_DrvEntry();
    Set_RetVal_EventFlagCreate(KAL_ERR_NONE);

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaTMR_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    printf("TEST_AmbaTMR_DrvEntry\n");
}

void TEST_AmbaTMR_FindAvail(void)
{
    UINT32 TimerID;
    UINT32 *pTimerID = &TimerID;
    UINT32 TimeOut = 0U;
    AmbaTMR_FindAvail(pTimerID, TimeOut);

    AmbaTMR_FindAvail(NULL, TimeOut);

    Set_RetVal_EventFlagGet(KAL_ERR_0000);
    AmbaTMR_FindAvail(pTimerID, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_TIMEOUT);
    AmbaTMR_FindAvail(pTimerID, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_NONE);

    printf("TEST_AmbaTMR_FindAvail\n");
}

void TEST_AmbaTMR_Acquire(void)
{
    UINT32 TimerID = AMBA_NUM_AVAIL_TIMER - 1U;
    UINT32 TimeOut = 0U;
    AmbaTMR_Acquire(TimerID, TimeOut);

    Set_RetVal_EventFlagGet(KAL_ERR_0000);
    AmbaTMR_Acquire(TimerID, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_TIMEOUT);
    AmbaTMR_Acquire(TimerID, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_NONE);

    TimerID = AMBA_NUM_AVAIL_TIMER;
    AmbaTMR_Acquire(TimerID, TimeOut);

    printf("TEST_AmbaTMR_Acquire\n");
}

void TEST_AmbaTMR_Release(void)
{
    UINT32 TimerID = AMBA_NUM_AVAIL_TIMER - 1U;

    /* avoid access invalid address in AmbaCSL_TmrGetEnable */
    pAmbaTMR_Regs[0]->TimerCtrl0 = 0U;
    pAmbaTMR_Regs[0]->TimerCtrl1 = 0U;
    pAmbaTMR_Regs[1]->TimerCtrl0 = 0U;
    pAmbaTMR_Regs[1]->TimerCtrl1 = 0U;

    AmbaTMR_Release(TimerID);

    Set_RetVal_EventFlagSet(KAL_ERR_0000);
    AmbaTMR_Release(TimerID);
    Set_RetVal_EventFlagSet(KAL_ERR_NONE);

    TimerID = 0U;
    pAmbaTMR_Regs[0]->TimerCtrl0 = 1U;
    AmbaTMR_Release(TimerID);

    TimerID = AMBA_NUM_AVAIL_TIMER;
    AmbaTMR_Release(TimerID);

    printf("TEST_AmbaTMR_Release\n");
}


void TEST_AmbaTMR_HookTimeOutHandler(void)
{
    UINT32 TimerID = AMBA_NUM_AVAIL_TIMER - 1U;
    AMBA_TMR_ISR_f ExpireFunc = 0U;
    UINT32 ExpireFuncArg = 0U;
    AmbaTMR_HookTimeOutHandler(TimerID, ExpireFunc, ExpireFuncArg);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaTMR_HookTimeOutHandler(TimerID, ExpireFunc, ExpireFuncArg);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaTMR_HookTimeOutHandler(TimerID, ExpireFunc, ExpireFuncArg);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    TimerID = 0U;
    pAmbaTMR_Regs[0]->TimerCtrl0 = 1U;
    AmbaTMR_HookTimeOutHandler(TimerID, ExpireFunc, ExpireFuncArg);

    TimerID = AMBA_NUM_AVAIL_TIMER;
    AmbaTMR_HookTimeOutHandler(TimerID, ExpireFunc, ExpireFuncArg);

    /* AmbaRTSL_TMR.c */
    AmbaRTSL_TmrHookTimeOutHandler(TimerID, ExpireFunc, ExpireFuncArg);

    printf("TEST_AmbaTMR_HookTimeOutHandler\n");
}


void TEST_AmbaTMR_Config(void)
{
    UINT32 TimerID = AMBA_NUM_AVAIL_TIMER - 1U;
    UINT32 TimerFreq = 1U;
    UINT32 NumPeriodicTick = 0U;
    AmbaTMR_Config(TimerID, TimerFreq, NumPeriodicTick);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaTMR_Config(TimerID, TimerFreq, NumPeriodicTick);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaTMR_Config(TimerID, TimerFreq, NumPeriodicTick);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    TimerID = 0U;
    pAmbaTMR_Regs[0]->TimerCtrl0 = 1U;
    AmbaTMR_Config(TimerID, TimerFreq, NumPeriodicTick);

    Set_RetVal_PllGetApbClk(1U);
    AmbaTMR_Config(TimerID, TimerFreq, NumPeriodicTick);

    TimerFreq = 0U;
    AmbaTMR_Config(TimerID, TimerFreq, NumPeriodicTick);

    /* AmbaRTSL_TMR.c */
    AmbaRTSL_TmrConfig(TimerID, TimerFreq, NumPeriodicTick);

    TimerID = AMBA_NUM_AVAIL_TIMER;
    AmbaTMR_Config(TimerID, TimerFreq, NumPeriodicTick);

    /* AmbaRTSL_TMR.c */
    AmbaRTSL_TmrConfig(TimerID, TimerFreq, NumPeriodicTick);

    TimerFreq = 1U;
    TimerID = AMBA_NUM_AVAIL_TIMER - 1U;
    AmbaTMR_Config(TimerID, TimerFreq, NumPeriodicTick);

    printf("TEST_AmbaTMR_Config\n");
}


void TEST_AmbaTMR_Start(void)
{
    UINT32 TimerID = AMBA_NUM_AVAIL_TIMER - 1U;
    UINT32 NumPeriodicTick = 0U;
    AmbaTMR_Start(TimerID, NumPeriodicTick);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaTMR_Start(TimerID, NumPeriodicTick);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaTMR_Start(TimerID, NumPeriodicTick);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    TimerID = AMBA_NUM_AVAIL_TIMER;
    AmbaTMR_Start(TimerID, NumPeriodicTick);

    /* AmbaRTSL_TMR.c */
    AmbaRTSL_TmrStart(TimerID, NumPeriodicTick);

    printf("TEST_AmbaTMR_Start\n");
}


void TEST_AmbaTMR_Stop(void)
{
    UINT32 TimerID = AMBA_NUM_AVAIL_TIMER - 1U;
    AmbaTMR_Stop(TimerID);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaTMR_Stop(TimerID);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaTMR_Stop(TimerID);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    TimerID = AMBA_NUM_AVAIL_TIMER;
    AmbaTMR_Stop(TimerID);

    /* AmbaRTSL_TMR.c */
    AmbaRTSL_TmrStop(TimerID);

    printf("TEST_AmbaTMR_Stop\n");
}


void TEST_AmbaTMR_WaitTimeExpired(void)
{
    UINT32 TimerID = AMBA_NUM_AVAIL_TIMER - 1U;
    UINT32 TimeOut = 0U;
    UINT32 TimerFreq = 1U;
    UINT32 NumPeriodicTick = 0U;
    AmbaTMR_WaitTimeExpired(TimerID, TimeOut);

    TimerID = 0U;
    pAmbaTMR_Regs[0]->TimerCtrl0 = 1U;
    AmbaTMR_Config(TimerID, TimerFreq, NumPeriodicTick);
    AmbaTMR_WaitTimeExpired(TimerID, TimeOut);

    Set_RetVal_EventFlagGet(KAL_ERR_0000);
    AmbaTMR_WaitTimeExpired(TimerID, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_TIMEOUT);
    AmbaTMR_WaitTimeExpired(TimerID, TimeOut);
    Set_RetVal_EventFlagGet(KAL_ERR_NONE);

    TimerID = AMBA_NUM_AVAIL_TIMER;
    AmbaTMR_WaitTimeExpired(TimerID, TimeOut);

    printf("TEST_AmbaTMR_WaitTimeExpired\n");
}

void TEST_AmbaTMR_ShowTickCount(void)
{
    UINT32 TimerID = AMBA_NUM_AVAIL_TIMER - 1U;
    UINT32 TimeLeft = 0U;
    UINT32 *pTimeLeft = &TimeLeft;
    AmbaTMR_ShowTickCount(TimerID, pTimeLeft);

    AmbaTMR_ShowTickCount(TimerID, NULL);

    /* AmbaRTSL_TMR.c */
    AmbaRTSL_TmrShowTickCount(TimerID, NULL);

    TimerID = AMBA_NUM_AVAIL_TIMER;
    AmbaTMR_ShowTickCount(TimerID, pTimeLeft);

    /* AmbaRTSL_TMR.c */
    AmbaRTSL_TmrShowTickCount(TimerID, pTimeLeft);

    printf("TEST_AmbaTMR_ShowTickCount\n");
}

void TEST_AmbaTMR_GetInfo(void)
{
    UINT32 TimerID = AMBA_NUM_AVAIL_TIMER - 1U;
    AMBA_TMR_INFO_s TimerInfo = {0};
    AMBA_TMR_INFO_s * pTimerInfo = &TimerInfo;
    AmbaTMR_GetInfo(TimerID, pTimerInfo);

    AmbaTMR_GetInfo(TimerID, NULL);

    /* AmbaRTSL_TMR.c */
    AmbaRTSL_TmrGetInfo(TimerID, NULL);

    TimerID = AMBA_NUM_AVAIL_TIMER;
    AmbaTMR_GetInfo(TimerID, pTimerInfo);

    /* AmbaRTSL_TMR.c */
    AmbaRTSL_TmrGetInfo(TimerID, pTimerInfo);

    printf("TEST_AmbaTMR_GetInfo\n");
}


int main(void)
{
    memcpy(&pAmbaTMR_Regs[0], &pTempAddr0, 8);
    memcpy(&pAmbaTMR_Regs[1], &pTempAddr1, 8);

    /* avoid AmbaTMR_DrvEntry to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaTMR_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaTMR_FindAvail();
    TEST_AmbaTMR_Acquire();
    TEST_AmbaTMR_Release();
    TEST_AmbaTMR_HookTimeOutHandler();
    TEST_AmbaTMR_Config();
    TEST_AmbaTMR_Start();
    TEST_AmbaTMR_Stop();
    TEST_AmbaTMR_WaitTimeExpired();
    TEST_AmbaTMR_ShowTickCount();
    TEST_AmbaTMR_GetInfo();

    AmbaRTSL_TmrInit();
    AmbaRTSL_TmrCoreFreqChg();

    return 0;
}

