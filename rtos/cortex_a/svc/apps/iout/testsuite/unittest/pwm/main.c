#include <stdio.h>
#include <stdlib.h>
#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaDrvEntry.h"
#include "AmbaPWM.h"
#include "AmbaRTSL_PWM.h"
#include "AmbaReg_PWM.h"

extern void Set_RetVal_EventFlagCreate(UINT32 RetVal);
extern void Set_RetVal_EventFlagClear(UINT32 RetVal);
extern void Set_RetVal_EventFlagGet(UINT32 RetVal);
extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);
extern void Set_RetVal_memset(UINT32 RetVal);
extern void Set_RetVal_PllGetPwmClk(UINT32 RetVal);

AMBA_PWM_REG_s *pAmbaPWM_Reg[3];

void TEST_AmbaPWM_DrvEntry(void)
{
    AmbaPWM_DrvEntry();

    Set_RetVal_EventFlagCreate(KAL_ERR_0000);
    AmbaPWM_DrvEntry();
    Set_RetVal_EventFlagCreate(KAL_ERR_NONE);

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaPWM_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    Set_RetVal_memset(1U);
    AmbaPWM_DrvEntry();
    Set_RetVal_memset(0U);

    printf("TEST_AmbaPWM_DrvEntry\n");
}

void TEST_AmbaPWM_Config(void)
{
    UINT32 PinPairID = AMBA_NUM_PWM_CHANNEL - 1U;
    UINT32 BaseFreq = 1U;
    AmbaPWM_Config(PinPairID, BaseFreq);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaPWM_Config(PinPairID, BaseFreq);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaPWM_Config(PinPairID, BaseFreq);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    BaseFreq = 0U;
    AmbaPWM_Config(PinPairID, BaseFreq);

    AmbaRTSL_PwmSetConfig(PinPairID, BaseFreq);

    PinPairID = AMBA_NUM_PWM_CHANNEL;
    AmbaPWM_Config(PinPairID, BaseFreq);

    AmbaRTSL_PwmSetConfig(PinPairID, BaseFreq);

    printf("TEST_AmbaPWM_Config\n");
}

void TEST_AmbaPWM_Start(void)
{
    UINT32 PinPairID = 0U;
    UINT32 Period = 1U;
    UINT32 Duty = 0U;
    UINT32 BaseFreq = 1U;
    AmbaPWM_Start(PinPairID, Period, Duty);

    for (PinPairID = AMBA_PWM_CHANNEL0; PinPairID < (AMBA_NUM_PWM_CHANNEL - 1U); PinPairID++) {
        AmbaPWM_Config(PinPairID, BaseFreq);
        AmbaPWM_Start(PinPairID, Period, Duty);
    }

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaPWM_Start(PinPairID, Period, Duty);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaPWM_Start(PinPairID, Period, Duty);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaPWM_Stop(PinPairID);
    AmbaPWM_Start(PinPairID, Period, Duty);

    Duty = 2U;
    AmbaPWM_Start(PinPairID, Period, Duty);

    AmbaRTSL_PwmStart(PinPairID, Period, Duty);

    Period = 0U;
    AmbaPWM_Start(PinPairID, Period, Duty);

    AmbaRTSL_PwmStart(PinPairID, Period, Duty);

    PinPairID = AMBA_NUM_PWM_CHANNEL;
    AmbaPWM_Start(PinPairID, Period, Duty);

    AmbaRTSL_PwmStart(PinPairID, Period, Duty);

    printf("TEST_AmbaPWM_Start\n");
}


void TEST_AmbaPWM_Stop(void)
{
    UINT32 PinPairID = AMBA_NUM_PWM_CHANNEL - 1U;
    AmbaPWM_Stop(PinPairID);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaPWM_Stop(PinPairID);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaPWM_Stop(PinPairID);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    PinPairID = AMBA_NUM_PWM_CHANNEL;
    AmbaPWM_Stop(PinPairID);

    printf("TEST_AmbaPWM_Stop\n");
}

void TEST_AmbaPWM_GetInfo(void)
{
    UINT32 PinPairID = AMBA_NUM_PWM_CHANNEL - 1U;
    UINT32 ActualBaseFreq = 0U;
    UINT32 *pActualBaseFreq = &ActualBaseFreq;
    AmbaPWM_GetInfo(PinPairID, pActualBaseFreq);

    AmbaPWM_GetInfo(PinPairID, NULL);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaPWM_GetInfo(PinPairID, pActualBaseFreq);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaPWM_GetInfo(PinPairID, pActualBaseFreq);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaRTSL_PwmGetInfo(PinPairID, NULL);

    PinPairID = AMBA_NUM_PWM_CHANNEL;
    AmbaPWM_GetInfo(PinPairID, pActualBaseFreq);

    AmbaRTSL_PwmGetInfo(PinPairID, pActualBaseFreq);

    printf("TEST_AmbaPWM_GetInfo\n");
    return;
}

void TEST_AmbaPWM_StepConfig(void)
{
    UINT32 PinGrpID = 1U;
    UINT32 BaseFreq = 1U;
    UINT32 PulseWidth = 1U;
    AmbaPWM_StepConfig(PinGrpID, BaseFreq, PulseWidth);

    // Set_RetVal_MutexGive(KAL_ERR_0000);
    // AmbaPWM_StepConfig(PinGrpID, BaseFreq, PulseWidth);
    // Set_RetVal_MutexGive(KAL_ERR_NONE);

    // Set_RetVal_MutexTake(KAL_ERR_0000);
    // AmbaPWM_StepConfig(PinGrpID, BaseFreq, PulseWidth);
    // Set_RetVal_MutexTake(KAL_ERR_NONE);

    // PulseWidth = 0U;
    // AmbaPWM_StepConfig(PinGrpID, BaseFreq, PulseWidth);

    // BaseFreq = 0U;
    // AmbaPWM_StepConfig(PinGrpID, BaseFreq, PulseWidth);

    // PinGrpID = AMBA_NUM_PWM_STEPPER_CHANNEL;
    // AmbaPWM_StepConfig(PinGrpID, BaseFreq, PulseWidth);

    printf("TEST_AmbaPWM_StepConfig\n");
}


void TEST_AmbaPWM_StepSetDuty(void)
{
    UINT32 PinGrpID = 1U;
    UINT32 PinID = 1U;
    UINT32 Duty = 1U;
    AmbaPWM_StepSetDuty(PinGrpID, PinID, Duty);

    printf("TEST_AmbaPWM_StepSetDuty\n");
}

void TEST_AmbaPWM_StepAct(void)
{
    UINT32 PinGrpID = 1U;
    AmbaPWM_StepAct(PinGrpID);

    printf("TEST_AmbaPWM_StepAct\n");
}


void TEST_AmbaPWM_StepGetInfo(void)
{
    UINT32 PinGrpID;
    UINT32 ActualBaseFreq = 0U;
    UINT32 *pActualBaseFreq = &ActualBaseFreq;
    AmbaPWM_StepGetInfo(PinGrpID, pActualBaseFreq);

    printf("TEST_AmbaPWM_StepGetInfo\n");
}

void TEST_AmbaRTSL_PwmHalCoreFreqChg(void)
{
    AmbaRTSL_PwmHalCoreFreqChg();

    printf("TEST_AmbaRTSL_PwmHalCoreFreqChg\n");
}

int main(void)
{
    for (UINT8 i = 0U; i < 3U; i++) {
        pAmbaPWM_Reg[i] = malloc(sizeof(AMBA_PWM_REG_s));
    }

    /* avoid AmbaPWM_DrvEntry to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaPWM_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaPWM_Config();
    TEST_AmbaPWM_Start();
    TEST_AmbaPWM_Stop();
    TEST_AmbaPWM_GetInfo();
    TEST_AmbaPWM_StepConfig();
    TEST_AmbaPWM_StepSetDuty();
    TEST_AmbaPWM_StepAct();
    TEST_AmbaPWM_StepGetInfo();

    TEST_AmbaRTSL_PwmHalCoreFreqChg();

    return 0;
}

