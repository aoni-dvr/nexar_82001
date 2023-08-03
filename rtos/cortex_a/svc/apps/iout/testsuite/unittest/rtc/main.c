#include <stdio.h>
#include <stdlib.h>
#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaDrvEntry.h"
#include "AmbaTime.h"
#include "AmbaRTC.h"
#include "AmbaReg_ScratchpadS.h"

AMBA_SCRATCHPAD_S_REG_s AmbaScratchpadS_Reg = {0};
AMBA_SCRATCHPAD_S_REG_s *const pAmbaScratchpadS_Reg = &AmbaScratchpadS_Reg;

extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_RetVal_GetSysTickCount(UINT32 RetVal);
extern void Set_RetVal_TaiDateTime2TimeStamp(UINT32 RetVal);
extern void Set_RetVal_TaiTimeStamp2DateTime(UINT32 RetVal);

static AMBA_RTC_SET_DATE_TIME_CB_f AmbaRtcSetDateTimeFunc;
static AMBA_RTC_SET_TIME_STAMP_CB_f AmbaRtcSetTimeStampFunc;
static AMBA_RTC_GET_DATE_TIME_CB_f AmbaRtcGetDateTimeFunc;
static AMBA_RTC_GET_TIME_STAMP_CB_f AmbaRtcGetTimeStampFunc;

void TEST_AmbaRTC_DrvEntry(void)
{
    AmbaRTC_DrvEntry();

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaRTC_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    Set_RetVal_GetSysTickCount(KAL_ERR_0000);
    AmbaRTC_DrvEntry();
    Set_RetVal_GetSysTickCount(KAL_ERR_NONE);

    printf("TEST_AmbaRTC_DrvEntry\n");
}

void TEST_AmbaRTC_Config(void)
{
    AMBA_RTC_SET_DATE_TIME_CB_f SetDateTimeFunc;
    AMBA_RTC_SET_TIME_STAMP_CB_f SetTimeStampFunc;
    AMBA_RTC_GET_DATE_TIME_CB_f GetDateTimeFunc;
    AMBA_RTC_GET_TIME_STAMP_CB_f GetTimeStampFunc;
    AmbaRTC_Config(SetDateTimeFunc, SetTimeStampFunc, GetDateTimeFunc, GetTimeStampFunc);

    AmbaRTC_Config(NULL, SetTimeStampFunc, GetDateTimeFunc, GetTimeStampFunc);

    AmbaRTC_Config(NULL, NULL, GetDateTimeFunc, GetTimeStampFunc);

    AmbaRTC_Config(SetDateTimeFunc, SetTimeStampFunc, NULL, GetTimeStampFunc);

    AmbaRTC_Config(SetDateTimeFunc, SetTimeStampFunc, NULL, NULL);

    printf("TEST_AmbaRTC_Config\n");
    return;
}

void TEST_AmbaRTC_SetSysTime(void)
{
    AMBA_RTC_SET_DATE_TIME_CB_f SetDateTimeFunc = AmbaRtcSetDateTimeFunc;
    AMBA_RTC_SET_TIME_STAMP_CB_f SetTimeStampFunc = AmbaRtcSetTimeStampFunc;
    AMBA_RTC_GET_DATE_TIME_CB_f GetDateTimeFunc = AmbaRtcGetDateTimeFunc;
    AMBA_RTC_GET_TIME_STAMP_CB_f GetTimeStampFunc = AmbaRtcGetTimeStampFunc;
    AmbaRTC_Config(SetDateTimeFunc, SetTimeStampFunc, GetDateTimeFunc, GetTimeStampFunc);

    AMBA_RTC_DATE_TIME_s CurrTime = {0};
    AMBA_RTC_DATE_TIME_s *pCurrTime = &CurrTime;
    AmbaRTC_SetSysTime(pCurrTime);

    Set_RetVal_TaiDateTime2TimeStamp(1U);
    AmbaRTC_SetSysTime(pCurrTime);
    Set_RetVal_TaiDateTime2TimeStamp(0U);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaRTC_SetSysTime(pCurrTime);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_GetSysTickCount(KAL_ERR_0000);
    AmbaRTC_SetSysTime(pCurrTime);
    Set_RetVal_GetSysTickCount(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaRTC_SetSysTime(pCurrTime);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaRTC_SetSysTime\n");
    return;
}


void TEST_AmbaRTC_GetSysTime(void)
{
    AMBA_RTC_SET_DATE_TIME_CB_f SetDateTimeFunc = AmbaRtcSetDateTimeFunc;
    AMBA_RTC_SET_TIME_STAMP_CB_f SetTimeStampFunc = AmbaRtcSetTimeStampFunc;
    AMBA_RTC_GET_DATE_TIME_CB_f GetDateTimeFunc = AmbaRtcGetDateTimeFunc;
    AMBA_RTC_GET_TIME_STAMP_CB_f GetTimeStampFunc = AmbaRtcGetTimeStampFunc;
    AmbaRTC_Config(SetDateTimeFunc, SetTimeStampFunc, GetDateTimeFunc, GetTimeStampFunc);

    AMBA_RTC_DATE_TIME_s CurrTime = {0};
    AMBA_RTC_DATE_TIME_s *pCurrTime = &CurrTime;
    AmbaRTC_GetSysTime(pCurrTime);

    Set_RetVal_TaiTimeStamp2DateTime(1U);
    AmbaRTC_GetSysTime(pCurrTime);
    Set_RetVal_TaiTimeStamp2DateTime(0U);

    printf("TEST_AmbaRTC_GetSysTime\n");
    return;
}

int main(void)
{
    TEST_AmbaRTC_DrvEntry();
    TEST_AmbaRTC_Config();
    TEST_AmbaRTC_SetSysTime();
    TEST_AmbaRTC_GetSysTime();

    return 0;
}

