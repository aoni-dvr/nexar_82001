#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDrvEntry.h"
#include "AmbaSYS.h"
#include "AmbaReg_ScratchpadS.h"
#include "AmbaCSL_RCT.h"

AMBA_SCRATCHPAD_S_REG_s AmbaScratchpadS_Reg = {0};
AMBA_SCRATCHPAD_S_REG_s *const pAmbaScratchpadS_Reg = &AmbaScratchpadS_Reg;
AMBA_RCT_REG_s AmbaRCT_Reg = {0};
AMBA_RCT_REG_s *const pAmbaRCT_Reg = &AmbaRCT_Reg;

extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_RetVal_PwcGetBootDevice(UINT32 RetVal);
extern void Set_RetVal_SdSendCMD0(UINT32 RetVal);
extern void Set_RetVal_WdtGetResetStatus(UINT32 RetVal);

void TEST_AmbaSYS_DrvEntry(void)
{
    AmbaSYS_DrvEntry();

    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaSYS_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    printf("TEST_AmbaSYS_DrvEntry\n");
}

void TEST_AmbaSYS_GetRtosCoreId(void)
{
    UINT32 CoreId = 0U;
    UINT32 *pCoreId = &CoreId;
    AmbaSYS_GetRtosCoreId(pCoreId);

    printf("TEST_AmbaSYS_GetRtosCoreId\n");
}

void TEST_AmbaSYS_SetExtInputClkInfo(void)
{
    UINT32 ExtClkID = 0U;
    UINT32 Freq = 0U;

    for (ExtClkID = 0x0U; ExtClkID <= 0x6U; ExtClkID ++)
    {
        AmbaSYS_SetExtInputClkInfo(ExtClkID, Freq);
    }

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSYS_SetExtInputClkInfo(ExtClkID, Freq);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSYS_SetExtInputClkInfo(ExtClkID, Freq);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaSYS_SetExtInputClkInfo\n");
}

void TEST_AmbaSYS_SetClkFreq(void)
{
    UINT32 ClkID = 0U;
    UINT32 DesiredFreq = 0U;
    UINT32 ActualFreq = 0U;
    UINT32 * pActualFreq = &ActualFreq;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    ClkID = AMBA_SYS_CLK_CORE;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    ClkID = AMBA_SYS_CLK_IDSP;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    ClkID = AMBA_SYS_CLK_VISION;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    ClkID = AMBA_SYS_CLK_AUD_0;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    ClkID = AMBA_SYS_CLK_VID_IN0;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    ClkID = AMBA_SYS_CLK_VID_IN1;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    ClkID = AMBA_SYS_CLK_VID_OUT0;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    ClkID = AMBA_SYS_CLK_VID_OUT1;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    ClkID = AMBA_SYS_CLK_REF_OUT0;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    ClkID = AMBA_SYS_CLK_REF_OUT1;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    ClkID = AMBA_SYS_CLK_FEX;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    ClkID = AMBA_SYS_CLK_SD;
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaSYS_SetClkFreq(ClkID, DesiredFreq, NULL);

    printf("TEST_AmbaSYS_SetClkFreq\n");
}

void TEST_AmbaSYS_GetClkFreq(void)
{
    UINT32 ClkID = 0U;
    UINT32 Freq = 0U;
    UINT32 * pFreq = &Freq;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_CORE;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_CORTEX;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_DRAM;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_IDSP;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_VISION;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_DSP_SYS;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_AUD_0;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_VID_IN0;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_VID_IN1;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_VID_OUT0;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_VID_OUT1;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_REF_OUT0;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_REF_OUT1;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_FEX;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    ClkID = AMBA_SYS_CLK_SD;
    AmbaSYS_GetClkFreq(ClkID, pFreq);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSYS_GetClkFreq(ClkID, pFreq);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSYS_GetClkFreq(ClkID, pFreq);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaSYS_GetClkFreq(ClkID, NULL);

    printf("TEST_AmbaSYS_GetClkFreq\n");
}

void TEST_AmbaSYS_SetIoClkFreq(void)
{
    UINT32 ClkID = 0U;
    UINT32 DesiredFreq = 0U;
    UINT32 ActualFreq = 0U;
    UINT32 * pActualFreq = &ActualFreq;
    AmbaSYS_SetIoClkFreq(ClkID, DesiredFreq, pActualFreq);

    printf("TEST_AmbaSYS_SetIoClkFreq\n");
}

void TEST_AmbaSYS_GetIoClkFreq(void)
{
    UINT32 ClkID = 0U;
    UINT32 Freq = 0U;
    UINT32 * pFreq = &Freq;
    AmbaSYS_GetIoClkFreq(ClkID, pFreq);

    printf("TEST_AmbaSYS_GetIoClkFreq\n");
}

void TEST_AmbaSYS_SetMaxIdleCycleTime(void)
{
    UINT32 MaxIdleCycleTime = 0U;
    AmbaSYS_SetMaxIdleCycleTime(MaxIdleCycleTime);

    printf("TEST_AmbaSYS_SetMaxIdleCycleTime\n");
}

void TEST_AmbaSYS_GetMaxIdleCycleTime(void)
{
    UINT32 MaxIdleCycleTime = 0U;
    UINT32 *pMaxIdleCycleTime = &MaxIdleCycleTime;
    AmbaSYS_GetMaxIdleCycleTime(pMaxIdleCycleTime);

    AmbaSYS_GetMaxIdleCycleTime(NULL);

    printf("TEST_AmbaSYS_GetMaxIdleCycleTime\n");
}

void TEST_AmbaSYS_Reboot(void)
{
    AmbaSYS_Reboot();

    Set_RetVal_PwcGetBootDevice(AMBA_SYS_BOOT_FROM_NAND);
    AmbaSYS_Reboot();

    Set_RetVal_PwcGetBootDevice(AMBA_SYS_BOOT_FROM_EMMC);
    AmbaSYS_Reboot();

    Set_RetVal_SdSendCMD0(1);
    AmbaSYS_Reboot();
    Set_RetVal_SdSendCMD0(0);

    Set_RetVal_PwcGetBootDevice(AMBA_SYS_BOOT_FROM_SPI_NOR);
    AmbaSYS_Reboot();

    printf("TEST_AmbaSYS_Reboot\n");
}

void TEST_AmbaSYS_ChangePowerMode(void)
{
    UINT32 PowerMode = 0U;

    for (PowerMode = 0U; PowerMode <= 2U; PowerMode ++)
    {
        AmbaSYS_ChangePowerMode(PowerMode);
    }

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSYS_ChangePowerMode(PowerMode);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSYS_ChangePowerMode(PowerMode);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaSYS_ChangePowerMode\n");
}

void TEST_AmbaSYS_GetBootMode(void)
{
    UINT32 BootMode = 0U;
    UINT32 *pBootMode = &BootMode;
    AmbaSYS_GetBootMode(pBootMode);

    AmbaSYS_GetBootMode(NULL);

    printf("TEST_AmbaSYS_GetBootMode\n");
}

void TEST_AmbaSYS_EnableFeature(void)
{
    UINT32 SysFeature = 0U;
    AmbaSYS_EnableFeature(SysFeature);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSYS_EnableFeature(SysFeature);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSYS_EnableFeature(SysFeature);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaSYS_EnableFeature\n");
}

void TEST_AmbaSYS_DisableFeature(void)
{
    UINT32 SysFeature = 0U;
    AmbaSYS_DisableFeature(SysFeature);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaSYS_DisableFeature(SysFeature);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaSYS_DisableFeature(SysFeature);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    printf("TEST_AmbaSYS_DisableFeature\n");
}

void TEST_AmbaSYS_CheckFeature(void)
{
    UINT32 SysFeature = 0U;
    UINT32 Enable = 0U;
    UINT32 *pEnable = &Enable;
    AmbaSYS_CheckFeature(SysFeature, pEnable);

    AmbaSYS_CheckFeature(SysFeature, NULL);

    printf("TEST_AmbaSYS_CheckFeature\n");
}

void TEST_AmbaSYS_GetOrcTimer(void)
{
    UINT32 OrcTimer = 0U;
    UINT32 *pOrcTimer = &OrcTimer;
    AmbaSYS_GetOrcTimer(pOrcTimer);

    AmbaSYS_GetOrcTimer(NULL);

    printf("TEST_AmbaSYS_GetOrcTimer\n");
}

void TEST_AmbaSYS_JtagOn(void)
{
    AmbaSYS_JtagOn();

    printf("TEST_AmbaSYS_JtagOn\n");
}

void TEST_AmbaSYS_GetWakeUpInfo(void)
{
    UINT32 WakeUpInfo = 0U;
    UINT32 *pWakeUpInfo = &WakeUpInfo;
    AmbaSYS_GetWakeUpInfo(pWakeUpInfo);

    Set_RetVal_WdtGetResetStatus(0x1U);
    AmbaSYS_GetWakeUpInfo(pWakeUpInfo);
    Set_RetVal_WdtGetResetStatus(0x0U);

    printf("TEST_AmbaSYS_GetWakeUpInfo\n");
}


int main(void)
{
    TEST_AmbaSYS_DrvEntry();
    TEST_AmbaSYS_GetRtosCoreId();
    TEST_AmbaSYS_SetExtInputClkInfo();
    TEST_AmbaSYS_SetClkFreq();
    TEST_AmbaSYS_GetClkFreq();
    TEST_AmbaSYS_SetIoClkFreq();
    TEST_AmbaSYS_GetIoClkFreq();
    TEST_AmbaSYS_SetMaxIdleCycleTime();
    TEST_AmbaSYS_GetMaxIdleCycleTime();
    TEST_AmbaSYS_Reboot();
    TEST_AmbaSYS_ChangePowerMode();
    TEST_AmbaSYS_GetBootMode();
    TEST_AmbaSYS_EnableFeature();
    TEST_AmbaSYS_DisableFeature();
    TEST_AmbaSYS_CheckFeature();
    TEST_AmbaSYS_GetOrcTimer();
    TEST_AmbaSYS_JtagOn();
    TEST_AmbaSYS_GetWakeUpInfo();

    return 0;
}

