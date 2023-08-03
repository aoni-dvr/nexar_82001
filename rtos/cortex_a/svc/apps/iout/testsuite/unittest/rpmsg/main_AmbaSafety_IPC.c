#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaTypes.h"
#include "AmbaCortexA53.h"

#include "src/AmbaSafety_IPC.c"

#define AMBA_COPY     (0U)
#define AMBA_NOCOPY   (1U)
#define AMBA_SAFECOPY (2U)

AMBA_LINK_TASK_CTRL_s AmbaLinkTaskCtrl[LINK_TASK_MAX] = {
    [LINK_TASK_INIT] = {
        .TaskName   = "LINK_TASK",
        .Priority   = 115,
        .StackSize  = 0x2000,
        .CoreSel    = 0x1,
    }
};

extern void Set_RetVal_TaskCreate(UINT32 RetVal);
extern void Set_RetVal_TaskResume(UINT32 RetVal);
extern void Set_RetVal_TaskSetSmpAffinity(UINT32 RetVal);
extern void Set_RetVal_SemaphoreCreate(UINT32 RetVal);
extern void Set_RetVal_TimerCreate(UINT32 RetVal);
extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_NoCopy_AmbaWrap_memset(UINT32 NoCopy);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);

/****************************************************/
/*                init Func                         */
/****************************************************/

/****************************************************/
/*                Local Stub Func                   */
/****************************************************/
UINT32 IO_UtilityUInt32ToStr(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
{
    return 0U;
}

UINT32 AmbaIPC_INTDisable(void)
{
    return 0U;
}

/****************************************************/
/*                Main Test                         */
/****************************************************/
void TEST_AmbaSafety_IPCSetSafeState(void)
{
    printf("TEST_AmbaSafety_IPCSetSafeState 1\n");
    /* arrange */
    /* act */
    AmbaSafety_IPCSetSafeState(0U);

    printf("TEST_AmbaSafety_IPCSetSafeState done\n");
}

void TEST_AmbaSafety_IPCGetSafeState(void)
{
    UINT32 State = 0U;

    printf("TEST_AmbaSafety_IPCGetSafeState 1\n");
    /* arrange */
    /* act */
    AmbaSafety_IPCGetSafeState(NULL);

    printf("TEST_AmbaSafety_IPCGetSafeState 2\n");
    /* arrange */
    /* act */
    AmbaSafety_IPCGetSafeState(&State);

    printf("TEST_AmbaSafety_IPCGetSafeState done\n");
}

void TEST_AmbaSafety_IPCEnq(void)
{
    printf("TEST_AmbaSafety_IPCEnq 1\n");
    /* arrange */
    /* act */
    AmbaSafety_IPCEnq(0U);

    printf("TEST_AmbaSafety_IPCEnq done\n");
}

void TEST_AmbaSafety_IPCDeq(void)
{
    printf("TEST_AmbaSafety_IPCDeq 1\n");
    /* arrange */
    /* act */
    AmbaSafety_IPCDeq(0U);

    printf("TEST_AmbaSafety_IPCDeq done\n");
}

void TEST_AmbaSafety_IPCIrqHdlr(void)
{
    printf("TEST_AmbaSafety_IPCIrqHdlr 1\n");
    /* arrange */
    /* act */
    AmbaSafety_IPCIrqHdlr();

    printf("TEST_AmbaSafety_IPCIrqHdlr done\n");
}

// void TEST_AmbaSafety_IPCAsilChecker(void)
// {
//     //UINT32 *ModuleID, UINT32 *InstanceID, UINT32 *ApiID, UINT32 *ErrorID
//     printf("TEST_AmbaSafety_IPCAsilChecker 1\n");
//     /* arrange */
//     /* act */
//     AmbaSafety_IPCAsilChecker(NULL, NULL, NULL, NULL);

//     printf("TEST_AmbaSafety_IPCAsilChecker done\n");
// }

void TEST_AmbaSafety_IPCInit(void)
{
    printf("TEST_AmbaSafety_IPCInit 1\n");
    /* arrange */
    // if (Ret != 0U)
    Set_RetVal_SemaphoreCreate(1U);
    Set_RetVal_TimerCreate(1U);
    Set_RetVal_MutexCreate(1U);
    // for if (err != 0U) while calling IPCSafetyTaskCreate()
    Set_RetVal_TaskCreate(1U);
    Set_RetVal_TaskResume(1U);
    Set_RetVal_TaskSetSmpAffinity(1U);
    // for cast pAmbaIPCShadow
    Set_NoCopy_AmbaWrap_memset(AMBA_NOCOPY);
    Set_NoCopy_AmbaMisra_TypeCast(AMBA_SAFECOPY);
    /* act */
    AmbaSafety_IPCInit();
    Set_RetVal_TaskCreate(0U);
    Set_RetVal_TaskResume(0U);
    Set_RetVal_TaskSetSmpAffinity(0U);
    Set_RetVal_SemaphoreCreate(0U);
    Set_RetVal_TimerCreate(0U);
    Set_RetVal_MutexCreate(0U);
    Set_NoCopy_AmbaWrap_memset(AMBA_COPY);
    Set_NoCopy_AmbaMisra_TypeCast(AMBA_COPY);

    printf("TEST_AmbaSafety_IPCInit 2\n");
    /* arrange */
    //if (Ret == 0U)
    Set_NoCopy_AmbaWrap_memset(AMBA_NOCOPY);
    Set_NoCopy_AmbaMisra_TypeCast(AMBA_SAFECOPY);
    /* act */
    AmbaSafety_IPCInit();
    Set_NoCopy_AmbaWrap_memset(AMBA_COPY);
    Set_NoCopy_AmbaMisra_TypeCast(AMBA_COPY);

    printf("TEST_AmbaSafety_IPCInit 3\n");
    /* arrange */
    // if(IPCW_Inited != 0U)
    /* act */
    AmbaSafety_IPCInit();

    printf("TEST_AmbaSafety_IPCInit done\n");
}

void TEST_AmbaSafety_IPCQmChecker(void)
{
    printf("TEST_AmbaSafety_IPCQmChecker 1\n");
    /* arrange */
    /* act */
    AmbaSafety_IPCQmChecker();

    printf("TEST_AmbaSafety_IPCQmChecker done\n");

}

void TEST_IPCW_Q_Check(void)
{
    printf("TEST_IPCW_Q_Check 1\n");
    /* arrange */
    /* act */
    IPCW_Q_Check();

    printf("TEST_IPCW_Q_Check done\n");

}

void TEST_IPCTimerIsrFunc(void)
{
    printf("TEST_IPCTimerIsrFunc 1\n");
    /* arrange */
    /* act */
    IPCTimerIsrFunc(0U);

    printf("TEST_IPCTimerIsrFunc done\n");

}

void TEST_IPCSafetyTaskEntry(void)
{
    printf("TEST_IPCSafetyTaskEntry 1\n");
    /* arrange */
    /* act */
    IPCSafetyTaskEntry(NULL);

    printf("TEST_IPCSafetyTaskEntry done\n");

}

int main(void)
{
    TEST_AmbaSafety_IPCInit(); // must in the begin of the test to init pIPCHeartBeat...etc.
    TEST_AmbaSafety_IPCSetSafeState();
    TEST_AmbaSafety_IPCGetSafeState();
    TEST_AmbaSafety_IPCEnq();
    TEST_AmbaSafety_IPCDeq();
    TEST_AmbaSafety_IPCIrqHdlr();
    TEST_AmbaSafety_IPCQmChecker();
    // TEST_AmbaSafety_IPCAsilChecker();

    TEST_IPCW_Q_Check();
    TEST_IPCTimerIsrFunc();
    TEST_IPCSafetyTaskEntry(); //TODO: infinite loop
    return 0;
}
