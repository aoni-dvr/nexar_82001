#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaDrvEntry.h"
#include "AmbaGPIO.h"
#include "AmbaKAL.h"
#include "AmbaRTSL_GPIO.h"
#include "AmbaReg_GPIO.h"
#include "AmbaReg_IoMux.h"
#include "AmbaReg_ScratchpadNS.h"
#include "AmbaReg_RCT.h"

AMBA_GPIO_REG_s AmbaGPIO_Reg[AMBA_NUM_GPIO_GROUP];
AMBA_GPIO_REG_s * pAmbaGPIO_Reg[AMBA_NUM_GPIO_GROUP];
AMBA_IO_MUX_REG_s AmbaIOMUX_Reg = {0};
AMBA_IO_MUX_REG_s *const pAmbaIOMUX_Reg = &AmbaIOMUX_Reg;
AMBA_SCRATCHPAD_NS_REG_s AmbaScratchpadNS_Reg = {0};
AMBA_SCRATCHPAD_NS_REG_s *const pAmbaScratchpadNS_Reg = &AmbaScratchpadNS_Reg;
AMBA_RCT_REG_s AmbaRCT_Reg = {0};
AMBA_RCT_REG_s *const pAmbaRCT_Reg = &AmbaRCT_Reg;

extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern UINT32 AmbaRTSL_GpioIntHookHandler(UINT32 PinID, AMBA_GPIO_ISR_f IntFunc, UINT32 IntFuncArg);
extern UINT32 AmbaRTSL_GpioIntSetType(UINT32 PinID, UINT32 IntType);
extern UINT32 AmbaRTSL_GpioIntEnable(UINT32 PinID);
extern UINT32 AmbaRTSL_GpioIntDisable(UINT32 PinID);
extern UINT32 AmbaRTSL_GpioIntClear(UINT32 PinID);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);
extern void Set_NoCopy_AmbaWrap_memcpy(UINT32 NoCopy);

void TEST_AmbaGPIO_LoadDefaultRegVals(void)
{
    AMBA_GPIO_DEFAULT_s DefaultParam = {0};
    AMBA_GPIO_DEFAULT_s *pDefaultParam = &DefaultParam;
    AmbaGPIO_LoadDefaultRegVals(NULL);

    /* avoid AmbaRTSL_GpioLoadDefaultVals to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    AmbaGPIO_LoadDefaultRegVals(pDefaultParam);

    pAmbaGPIO_Reg[3]->IntStatus = 1U;
    AmbaGPIO_LoadDefaultRegVals(pDefaultParam);

    Set_NoCopy_AmbaMisra_TypeCast(0U);

    printf("TEST_AmbaGPIO_LoadDefaultRegVals\n");
}

void TEST_AmbaGPIO_SetFuncGPI(void)
{
    UINT32 PinID = AMBA_NUM_GPIO_PIN - 1U;
    AmbaGPIO_SetFuncGPI(PinID);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGPIO_SetFuncGPI(PinID);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGPIO_SetFuncGPI(PinID);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    PinID = AMBA_NUM_GPIO_PIN;
    AmbaGPIO_SetFuncGPI(PinID);

    printf("TEST_AmbaGPIO_SetFuncGPI\n");
}

void TEST_AmbaGPIO_SetFuncGPO(void)
{
    UINT32 PinID = AMBA_NUM_GPIO_PIN - 1U;
    UINT32 PinLevel = GPIO_LV_LOW;
    AmbaGPIO_SetFuncGPO(PinID, PinLevel);

    PinLevel = GPIO_LV_HIGH;
    AmbaGPIO_SetFuncGPO(PinID, PinLevel);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGPIO_SetFuncGPO(PinID, PinLevel);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGPIO_SetFuncGPO(PinID, PinLevel);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    PinID = AMBA_NUM_GPIO_PIN;
    AmbaGPIO_SetFuncGPO(PinID, PinLevel);

    printf("TEST_AmbaGPIO_SetFuncGPO\n");
}

void TEST_AmbaGPIO_SetFuncAlt(void)
{
    UINT32 PinID = AMBA_NUM_GPIO_PIN - 1U;
    AmbaGPIO_SetFuncAlt(PinID);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGPIO_SetFuncAlt(PinID);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGPIO_SetFuncAlt(PinID);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    PinID = AMBA_NUM_GPIO_PIN;
    AmbaGPIO_SetFuncAlt(PinID);

    /* AmbaRTSL_GPIO.c */
    PinID = AMBA_NUM_GPIO_FUNC << 12U;
    AmbaRTSL_GpioSetFuncAlt(PinID);
   
    PinID = (AMBA_NUM_GPIO_FUNC - GPIO_FUNC_ALT1) << 12U;
    AmbaRTSL_GpioSetFuncAlt(PinID);
    
    PinID = AMBA_NUM_GPIO_PIN;
    AmbaRTSL_GpioSetFuncAlt(PinID);

    printf("TEST_AmbaGPIO_SetFuncAlt\n");
}

void TEST_AmbaGPIO_SetDriveStrength(void)
{
    UINT32 PinID = AMBA_NUM_GPIO_PIN - 1U;
    UINT32 DriveStrength = 1U;
    AmbaGPIO_SetDriveStrength(PinID, DriveStrength);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGPIO_SetDriveStrength(PinID, DriveStrength);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGPIO_SetDriveStrength(PinID, DriveStrength);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    PinID = AMBA_NUM_GPIO_PIN;
    AmbaGPIO_SetDriveStrength(PinID, DriveStrength);

    /* AmbaRTSL_GPIO.c */
    AmbaRTSL_GpioSetDriveStrength(PinID, DriveStrength);

    /* AmbaCSL_GPIO.c */
    PinID = AMBA_GPIO_GROUP0 << 5U;
    AmbaRTSL_GpioSetDriveStrength(PinID, DriveStrength);

    PinID = AMBA_GPIO_GROUP1 << 5U;
    AmbaRTSL_GpioSetDriveStrength(PinID, DriveStrength);

    PinID = AMBA_GPIO_GROUP2 << 5U;
    AmbaRTSL_GpioSetDriveStrength(PinID, DriveStrength);

    PinID = AMBA_GPIO_GROUP3 << 5U;
    AmbaRTSL_GpioSetDriveStrength(PinID, DriveStrength);

    DriveStrength = GPIO_DRV_STR_L0;
    AmbaRTSL_GpioSetDriveStrength(PinID, DriveStrength);

    DriveStrength = GPIO_DRV_STR_L1;
    AmbaRTSL_GpioSetDriveStrength(PinID, DriveStrength);

    DriveStrength = GPIO_DRV_STR_L2;
    AmbaRTSL_GpioSetDriveStrength(PinID, DriveStrength);

    DriveStrength = GPIO_DRV_STR_L3;
    AmbaRTSL_GpioSetDriveStrength(PinID, DriveStrength);

    printf("TEST_AmbaGPIO_SetDriveStrength\n");
}


void TEST_AmbaGPIO_SetPullUpOrDown(void)
{
    UINT32 PinID = AMBA_NUM_GPIO_PIN - 1U;
    UINT32 PullUpOrDown = 1U;
    AmbaGPIO_SetPullUpOrDown(PinID, PullUpOrDown);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGPIO_SetPullUpOrDown(PinID, PullUpOrDown);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGPIO_SetPullUpOrDown(PinID, PullUpOrDown);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    PinID = AMBA_NUM_GPIO_PIN;
    AmbaGPIO_SetPullUpOrDown(PinID, PullUpOrDown);

    /* AmbaRTSL_GPIO.c */
    AmbaRTSL_GpioSetPullUpOrDown(PinID, PullUpOrDown);

    printf("TEST_AmbaGPIO_SetPullUpOrDown\n");
}


void TEST_AmbaGPIO_IntSetType(void)
{
    UINT32 PinID = AMBA_NUM_GPIO_PIN - 1U;
    UINT32 IntConfig = 0U;
    for (IntConfig = 0U; IntConfig <= 5U; IntConfig++) { 
        AmbaGPIO_IntSetType(PinID, IntConfig);
    }

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGPIO_IntSetType(PinID, IntConfig);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGPIO_IntSetType(PinID, IntConfig);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    PinID = AMBA_NUM_GPIO_PIN;
    AmbaGPIO_IntSetType(PinID, IntConfig);

    /* AmbaRTSL_GPIO.c */
    AmbaRTSL_GpioIntSetType(PinID, IntConfig);

    printf("TEST_AmbaGPIO_IntSetType\n");
}


void TEST_AmbaGPIO_IntHookHandler(void)
{
    UINT32 PinID = AMBA_NUM_GPIO_PIN - 1U;
    AMBA_GPIO_ISR_f IntFunc;
    UINT32 IntFuncArg = 1U;
    AmbaGPIO_IntHookHandler(PinID, NULL, IntFuncArg);

    AmbaGPIO_IntHookHandler(PinID, IntFunc, IntFuncArg);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGPIO_IntHookHandler(PinID, IntFunc, IntFuncArg);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGPIO_IntHookHandler(PinID, IntFunc, IntFuncArg);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    PinID = AMBA_NUM_GPIO_PIN;
    AmbaGPIO_IntHookHandler(PinID, IntFunc, IntFuncArg);

    /* AmbaRTSL_GPIO.c */
    AmbaRTSL_GpioIntHookHandler(PinID, IntFunc, IntFuncArg);

    printf("TEST_AmbaGPIO_IntHookHandler\n");
}

void TEST_AmbaGPIO_IntEnable(void)
{
    UINT32 PinID = AMBA_NUM_GPIO_PIN - 1U;
    AmbaGPIO_IntEnable(PinID);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGPIO_IntEnable(PinID);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGPIO_IntEnable(PinID);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    PinID = AMBA_NUM_GPIO_PIN;
    AmbaGPIO_IntEnable(PinID);

    /* AmbaRTSL_GPIO.c */
    AmbaRTSL_GpioIntEnable(PinID);

    printf("TEST_AmbaGPIO_IntEnable\n");
}


void TEST_AmbaGPIO_IntDisable(void)
{
    UINT32 PinID = AMBA_NUM_GPIO_PIN - 1U;
    AmbaGPIO_IntDisable(PinID);

    /* AmbaRTSL_GPIO.c */
    pAmbaGPIO_Reg[3]->IntEnable = 1U;
    AmbaRTSL_GpioIntDisable(PinID);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaGPIO_IntDisable(PinID);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaGPIO_IntDisable(PinID);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    PinID = AMBA_NUM_GPIO_PIN;
    AmbaGPIO_IntDisable(PinID);

    /* AmbaRTSL_GPIO.c */
    AmbaRTSL_GpioIntDisable(PinID);

    printf("TEST_AmbaGPIO_IntDisable\n");
}

void TEST_AmbaGPIO_GetInfo(void)
{
    UINT32 PinID = AMBA_NUM_GPIO_PIN - 1U;
    AMBA_GPIO_INFO_s PinInfo = {0};
    AMBA_GPIO_INFO_s *pPinInfo = &PinInfo;
    AmbaGPIO_GetInfo(PinID, NULL);

    AmbaGPIO_GetInfo(PinID, pPinInfo);

    /* AmbaRTSL_GPIO.c */
    AmbaRTSL_GpioGetPinInfo(PinID, NULL);

    /* AmbaCSL_GPIO.c */
    pAmbaGPIO_Reg[3]->PinLevel = 0U;
    pAmbaGPIO_Reg[3]->IntEnable = (1U << 0x1FU);
    pAmbaGPIO_Reg[3]->IntTrigType0 = 0U;
    pAmbaGPIO_Reg[3]->IntTrigType2 = (1U << 0x1FU);
    AmbaGPIO_GetInfo(PinID, pPinInfo);

    pAmbaGPIO_Reg[3]->IntTrigType2 = 0U;
    AmbaGPIO_GetInfo(PinID, pPinInfo);

    pAmbaGPIO_Reg[3]->IntTrigType1 = (1U << 0x1FU);
    AmbaGPIO_GetInfo(PinID, pPinInfo);

    pAmbaGPIO_Reg[3]->IntTrigType0 = (1U << 0x1FU);
    AmbaGPIO_GetInfo(PinID, pPinInfo);

    pAmbaGPIO_Reg[3]->IntTrigType2 = (1U << 0x1FU);
    AmbaGPIO_GetInfo(PinID, pPinInfo);

    pAmbaGPIO_Reg[3]->IntTrigType2 = 0U;
    AmbaGPIO_GetInfo(PinID, pPinInfo);

    pAmbaGPIO_Reg[3]->PinDirection = 0U;
    AmbaGPIO_GetInfo(PinID, pPinInfo);

    PinID = AMBA_NUM_GPIO_PIN;
    AmbaGPIO_GetInfo(PinID, pPinInfo);

    /* AmbaRTSL_GPIO.c */
    PinID = AMBA_NUM_GPIO_FUNC << 12U;
    AmbaRTSL_GpioGetPinInfo(PinID, pPinInfo);

    PinID = (AMBA_NUM_GPIO_FUNC - GPIO_FUNC_ALT1) << 12U;
    AmbaRTSL_GpioGetPinInfo(PinID, pPinInfo);

    PinID = AMBA_NUM_GPIO_PIN;
    AmbaRTSL_GpioGetPinInfo(PinID, pPinInfo);

    /* AmbaCSL_GPIO.c */
    PinID = AMBA_GPIO_GROUP0 << 5U;
    AmbaRTSL_GpioGetPinInfo(PinID, pPinInfo);

    PinID = AMBA_GPIO_GROUP1 << 5U;
    AmbaRTSL_GpioGetPinInfo(PinID, pPinInfo);

    PinID = AMBA_GPIO_GROUP2 << 5U;
    AmbaRTSL_GpioGetPinInfo(PinID, pPinInfo);

    PinID = AMBA_GPIO_GROUP3 << 5U;
    AmbaRTSL_GpioGetPinInfo(PinID, pPinInfo);

    printf("TEST_AmbaGPIO_GetInfo\n");
}

void TEST_AmbaGPIO_DrvEntry(void)
{
    AmbaGPIO_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaGPIO_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    printf("TEST_AmbaGPIO_DrvEntry\n");
}

void TEST_AmbaRTSL_Gpio(void)
{
    UINT32 PinID = AMBA_NUM_GPIO_PIN - 1U;
    UINT8 i = 0U;
    AmbaRTSL_GpioCheckFunc(PinID);
    AmbaRTSL_GpioIntClear(PinID);

    PinID = (0x1U << 12U) + 0x1U;
    AmbaRTSL_GpioCheckFunc(PinID);
    
    PinID = AMBA_NUM_GPIO_PIN;
    AmbaRTSL_GpioCheckFunc(PinID);
    AmbaRTSL_GpioIntClear(PinID);

    printf("TEST_AmbaRTSL_Gpio\n");
}

int main(void)
{
    for (UINT8 i = 0U; i < AMBA_NUM_GPIO_GROUP; i++) {
        pAmbaGPIO_Reg[i] = malloc(sizeof(AMBA_GPIO_REG_s));
    }

    /* avoid AmbaGPIO_DrvEntry to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaGPIO_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaGPIO_LoadDefaultRegVals();
    TEST_AmbaGPIO_SetFuncGPI();
    TEST_AmbaGPIO_SetFuncGPO();
    TEST_AmbaGPIO_SetFuncAlt();
    TEST_AmbaGPIO_SetDriveStrength();
    TEST_AmbaGPIO_SetPullUpOrDown();
    TEST_AmbaGPIO_IntSetType();
    TEST_AmbaGPIO_IntHookHandler();
    TEST_AmbaGPIO_IntEnable();
    TEST_AmbaGPIO_IntDisable();
    TEST_AmbaGPIO_GetInfo();

    TEST_AmbaRTSL_Gpio();

    return 0;
}

