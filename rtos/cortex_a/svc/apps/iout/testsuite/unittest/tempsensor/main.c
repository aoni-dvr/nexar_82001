#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDrvEntry.h"
#include "AmbaTempSensor.h"
#include "AmbaRTSL_TempSensor.h"
#include "AmbaCSL_TempSensor.h"
#include "AmbaReg_TempSensor.h"


AMBA_TEMPSENSOR_REG_s AmbaTempSensor_Reg = {0};
AMBA_TEMPSENSOR_REG_s *pTempAddr = &AmbaTempSensor_Reg;
AMBA_TEMPSENSOR_REG_s *const pAmbaTempSensor_Reg;

extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);

void TEST_AmbaTempSensor_DrvEntry(void)
{
    AmbaTempSensor_DrvEntry();

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaTempSensor_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    printf("TEST_AmbaTempSensor_DrvEntry\n");
}

void TEST_AmbaTempSensor_GetTemp(void)
{
    UINT32 EnableDOC = 0U;
    UINT32 Channel = AMBA_TEMPSEN_NUM;
    DOUBLE Temp = 0;
    DOUBLE *pTemp = &Temp;
    AmbaTempSensor_GetTemp(EnableDOC, Channel, pTemp);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaTempSensor_GetTemp(EnableDOC, Channel, pTemp);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaTempSensor_GetTemp(EnableDOC, Channel, pTemp);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaTempSensor_GetTemp\n");
}

void TEST_AmbaTempSensor_GetTemp0(void)
{
    UINT32 EnableDOC = 0U;
    UINT32 Channel = 0U;
    DOUBLE Temp = 0;
    DOUBLE *pTemp = &Temp;
    AmbaTempSensor_GetTemp(EnableDOC, Channel, pTemp);

    printf("TEST_AmbaTempSensor_GetTemp0\n");
}

void TEST_AmbaTempSensor_GetTemp1(void)
{
    UINT32 EnableDOC = 0U;
    UINT32 Channel = AMBA_TEMPSEN_NUM - 1U;
    DOUBLE Temp = 0;
    DOUBLE *pTemp = &Temp;
    AmbaTempSensor_GetTemp(EnableDOC, Channel, pTemp);

    printf("TEST_AmbaTempSensor_GetTemp1\n");
}

void* func0(void* arg)
{
    // detach the current thread from the calling thread
    pthread_detach(pthread_self());

    TEST_AmbaTempSensor_GetTemp0();

    // exit the current thread
    pthread_exit(NULL);
}

void* func1(void* arg)
{
    // detach the current thread from the calling thread
    pthread_detach(pthread_self());

    TEST_AmbaTempSensor_GetTemp1();

    // exit the current thread
    pthread_exit(NULL);
}

int main(void)
{
    memcpy(&pAmbaTempSensor_Reg, &pTempAddr, 8);

    TEST_AmbaTempSensor_DrvEntry();
    TEST_AmbaTempSensor_GetTemp();

    // Creating a new thread
    pthread_t ptid0;
    pthread_t ptid1;

    pthread_create(&ptid0, NULL, &func0, NULL);

    sleep(2U);
    pAmbaTempSensor_Reg->TsEnable = 0x0U;

    pthread_join(ptid0, NULL);

    pthread_create(&ptid1, NULL, &func1, NULL);

    sleep(2U);
    pAmbaTempSensor_Reg->TsEnable = 0x0U;

    pthread_join(ptid1, NULL);

    pthread_exit(NULL);

    return 0;
}

