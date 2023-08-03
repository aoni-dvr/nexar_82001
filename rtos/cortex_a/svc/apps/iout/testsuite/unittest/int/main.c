#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDrvEntry.h"
#include "AmbaINT.h"
#include "AmbaRTSL_GIC.h"
#include "AmbaCSL_GIC.h"
#include "AmbaReg_GIC.h"

AMBA_GIC_DISTRIBUTOR_REG_s AmbaGIC_DistReg = {0};
AMBA_GIC_DISTRIBUTOR_REG_s * pAmbaGIC_DistReg = &AmbaGIC_DistReg;
AMBA_GIC_CPU_IF_REG_s      AmbaGIC_CpuIfReg = {0};
AMBA_GIC_CPU_IF_REG_s      * pAmbaGIC_CpuIfReg = &AmbaGIC_CpuIfReg;

extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);

void TEST_AmbaINT_DrvEntry(void)
{
    AmbaINT_DrvEntry();

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaINT_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    printf("TEST_AmbaINT_DrvEntry\n");
}

void TEST_AmbaINT_GlobalMaskEnable(void)
{
    AmbaINT_GlobalMaskEnable();

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaINT_GlobalMaskEnable();
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaINT_GlobalMaskEnable();
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaINT_GlobalMaskEnable\n");
}

void TEST_AmbaINT_GlobalMaskDisable(void)
{
    AmbaINT_GlobalMaskDisable();

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaINT_GlobalMaskDisable();
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaINT_GlobalMaskDisable();
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaINT_GlobalMaskDisable\n");
}

void TEST_AmbaINT_Config(void)
{
    UINT32 Irq = AMBA_NUM_INTERRUPT - 1U;
    AMBA_INT_CONFIG_s IntConfig = {0};
    AMBA_INT_CONFIG_s *pIntConfig = &IntConfig;
    AMBA_INT_ISR_f IntFunc;
    UINT32 IntFuncArg = 0U;
    AmbaINT_Config(Irq, pIntConfig, IntFunc, IntFuncArg);

    AmbaINT_Config(Irq, NULL, IntFunc, IntFuncArg);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaINT_Config(Irq, pIntConfig, IntFunc, IntFuncArg);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaINT_Config(Irq, pIntConfig, IntFunc, IntFuncArg);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    /* AmbaRTSL_GIC.c */
    AmbaRTSL_GicIntConfig(Irq, NULL, IntFunc, IntFuncArg);

    Irq = AMBA_INT_SPI_ID039_CAN0;
    AmbaINT_Config(Irq, pIntConfig, IntFunc, IntFuncArg);

    Irq = AMBA_NUM_INTERRUPT;
    AmbaINT_Config(Irq, pIntConfig, IntFunc, IntFuncArg);

    /* AmbaRTSL_GIC.c */
    AmbaRTSL_GicIntConfig(Irq, pIntConfig, IntFunc, IntFuncArg);

    printf("TEST_AmbaINT_Config\n");
}

void TEST_AmbaINT_Enable(void)
{
    UINT32 Irq = 0U;
    AmbaINT_Enable(Irq);

    Irq = AMBA_NUM_INTERRUPT;
    AmbaINT_Enable(Irq);

    printf("TEST_AmbaINT_Enable\n");
}

void TEST_AmbaINT_Disable(void)
{
    UINT32 Irq = 0U;
    AmbaINT_Disable(Irq);

    Irq = AMBA_NUM_INTERRUPT;
    AmbaINT_Disable(Irq);

    printf("TEST_AmbaINT_Disable\n");
}

void TEST_AmbaINT_SendSGI(void)
{
    UINT32 Irq = 0U;
    UINT32 SgiType = 0U;
    UINT32 CpuTargets = 0U;
    AmbaINT_SendSGI(Irq, SgiType, CpuTargets);

    AmbaRTSL_GicSendSGI(Irq, INT_TYPE_IRQ, SgiType, CpuTargets);

    SgiType = INT_NUM_SGI2CPU_TYPE;
    AmbaINT_SendSGI(Irq, SgiType, CpuTargets);

    Irq = AMBA_NUM_INTERRUPT;
    AmbaINT_SendSGI(Irq, SgiType, CpuTargets);

    printf("TEST_AmbaINT_SendSGI\n");
}

void TEST_AmbaINT_GetInfo(void)
{
    UINT32 Irq = AMBA_NUM_INTERRUPT - 1U;
    UINT32 GlobalIntEnable = 0U;
    UINT32 * pGlobalIntEnable = &GlobalIntEnable;
    AMBA_INT_INFO_s IrqInfo = {0};
    AMBA_INT_INFO_s *pIrqInfo = &IrqInfo;
    AmbaINT_GetInfo(Irq, pGlobalIntEnable, pIrqInfo);

    AmbaINT_GetInfo(Irq, NULL, pIrqInfo);

    AmbaINT_GetInfo(Irq, pGlobalIntEnable, NULL);

    /* AmbaRTSL_GIC.c */
    AmbaRTSL_GicGetIntInfo(Irq, NULL);

    Irq = AMBA_NUM_INTERRUPT;
    AmbaINT_GetInfo(Irq, pGlobalIntEnable, pIrqInfo);

    /* AmbaRTSL_GIC.c */
    AmbaRTSL_GicGetIntInfo(Irq, pIrqInfo);

    printf("TEST_AmbaINT_GetInfo\n");
}

void TEST_AmbaRTSL_GicSetCpuTarget(void)
{
    UINT32 IntID = 0U;
    UINT32 CpuTargetList = 0U;
    AmbaRTSL_GicSetCpuTarget(IntID, CpuTargetList);

    printf("TEST_AmbaRTSL_GicSetCpuTarget\n");
}

void TEST_AmbaRTSL_GicInit(void)
{
    /* avoid AmbaRTSL_GicInit to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    AmbaRTSL_GicInit();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    printf("TEST_AmbaRTSL_GicInit\n");
}

void TEST_AmbaRTSL_GicIsrIRQ(void)
{
    AmbaRTSL_GicIsrIRQ();

    printf("TEST_AmbaRTSL_GicIsrIRQ\n");
}

void TEST_AmbaRTSL_GicIsrFIQ(void)
{
    AmbaRTSL_GicIsrFIQ();

    printf("TEST_AmbaRTSL_GicIsrFIQ\n");
}

void TEST_AmbaRTSL_GicGetIntConfig(void)
{
    UINT32 IntID = 0U;
    AmbaRTSL_GicGetIntConfig(IntID);

    printf("TEST_AmbaRTSL_GicGetIntConfig\n");
}

void TEST_AmbaRTSL_GicGetISR(void)
{
    UINT32 IntID = 0U;
    AmbaRTSL_GicGetISR(IntID);

    IntID = AMBA_NUM_INTERRUPT;
    AmbaRTSL_GicGetISR(IntID);

    printf("TEST_AmbaRTSL_GicGetISR\n");
}

void TEST_AmbaRTSL_GicSetISR(void)
{
    UINT32 IntID = 0U;
    AMBA_INT_ISR_f IsrEntry;
    AmbaRTSL_GicSetISR(IntID, IsrEntry);

    IntID = AMBA_NUM_INTERRUPT;
    AmbaRTSL_GicSetISR(IntID, IsrEntry);

    printf("TEST_AmbaRTSL_GicSetISR\n");
}

void TEST_AmbaINT_SetProfileFunc(void)
{
    AMBA_INT_ISR_PROFILE_f EntryCb;
    AMBA_INT_ISR_PROFILE_f ExitCb;
    AmbaINT_SetProfileFunc(EntryCb, ExitCb);

    printf("TEST_AmbaINT_SetProfileFunc\n");
}

void TEST_AmbaCSL_GicSetIntPending(void)
{
    UINT32 IntID = 0U;
    AmbaCSL_GicSetIntPending(IntID);

    printf("TEST_AmbaCSL_GicSetIntPending\n");
}

void TEST_AmbaCSL_GicClearIntPending(void)
{
    UINT32 IntID = 0U;
    AmbaCSL_GicClearIntPending(IntID);

    printf("TEST_AmbaCSL_GicClearIntPending\n");
}

void TEST_AmbaCSL_GicSetIntActive(void)
{
    UINT32 IntID = 0U;
    AmbaCSL_GicSetIntActive(IntID);

    printf("TEST_AmbaCSL_GicSetIntActive\n");
}

void TEST_AmbaCSL_GicClearIntActive(void)
{
    UINT32 IntID = 0U;
    AmbaCSL_GicClearIntActive(IntID);

    printf("TEST_AmbaCSL_GicClearIntActive\n");
}

void TEST_AmbaCSL_GicSetIntTargetCore(void)
{
    UINT32 IntID = 0U;
    UINT32 CpuTargets = 0U;
    AmbaCSL_GicSetIntTargetCore(IntID, CpuTargets);

    printf("TEST_AmbaCSL_GicSetIntTargetCore\n");
}

void TEST_AmbaCSL_GicSetIntPriority(void)
{
    UINT32 IntID = 0U;
    UINT32 Priority = 0U;
    AmbaCSL_GicSetIntPriority(IntID, Priority);

    printf("TEST_AmbaCSL_GicSetIntPriority\n");
}

int main(void)
{
    TEST_AmbaINT_DrvEntry();
    TEST_AmbaINT_GlobalMaskEnable();
    TEST_AmbaINT_GlobalMaskDisable();
    TEST_AmbaINT_Config();
    TEST_AmbaINT_Enable();
    TEST_AmbaINT_Disable();
    TEST_AmbaINT_SendSGI();
    TEST_AmbaINT_GetInfo();

    TEST_AmbaRTSL_GicSetCpuTarget();
    TEST_AmbaRTSL_GicInit();
    TEST_AmbaRTSL_GicIsrFIQ();
    TEST_AmbaRTSL_GicIsrIRQ();
    TEST_AmbaRTSL_GicGetIntConfig();
    TEST_AmbaRTSL_GicGetISR();
    TEST_AmbaRTSL_GicSetISR();
    TEST_AmbaINT_SetProfileFunc();

    TEST_AmbaCSL_GicSetIntPending();
    TEST_AmbaCSL_GicClearIntPending();
    TEST_AmbaCSL_GicSetIntActive();
    TEST_AmbaCSL_GicClearIntActive();
    TEST_AmbaCSL_GicSetIntTargetCore();
    TEST_AmbaCSL_GicSetIntPriority();

    return 0;
}

