#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaTypes.h"

#include "src/AmbaIPC_SpinLock.c"

aspinlock_t lock[2];
AMBA_LINK_CTRL_s AmbaLinkCtrl = { .AmbaLinkMachineRev = 1U };

#define AMBA_COPY     (0U)
#define AMBA_NOCOPY   (1U)
extern void Set_NoCopy_AmbaWrap_memset(UINT32 NoCopy);

/****************************************************/
/*                Local Stub Func                   */
/****************************************************/
UINT32 AMBA_get_interrupt_state(void)
{

}

void AMBA_set_interrupt_state(UINT32 x)
{

}

void AMBA_disable_interrupt(void)
{

}

void do_SpinUnlock(UINT32 *lock)
{

}

void do_SpinLock(UINT32 *lock)
{

}

/****************************************************/
/*                Main Test                         */
/****************************************************/
void TEST_AmbaIPC_SpinInit(void)
{
    printf("TEST_AmbaIPC_SpinInit 1\n");
    /* arrange */
    AmbaLinkCtrl.AmbaIpcSpinLockBufAddr = (UINT32)lock;
    AmbaLinkCtrl.AmbaIpcSpinLockBufSize = sizeof(lock[0]); // = sizeof(aspinlock_t)
    /* act */
    AmbaIPC_SpinInit();

    printf("TEST_AmbaIPC_SpinInit 2\n");
    /* arrange */
    AmbaLinkCtrl.AmbaIpcSpinLockBufAddr = (UINT32)lock;
    AmbaLinkCtrl.AmbaIpcSpinLockBufSize = sizeof(lock); // = sizeof(aspinlock_t) * 2
    Set_NoCopy_AmbaWrap_memset(AMBA_NOCOPY);
    /* act */
    AmbaIPC_SpinInit();
    Set_NoCopy_AmbaWrap_memset(AMBA_COPY);

    printf("TEST_AmbaIPC_SpinInit done\n");
}

void TEST_AmbaIPC_SpinLockIrqSave(void)
{
    UINT32 SpinID;
    UINT32 *pFlags = malloc(sizeof(UINT32));

    printf("TEST_AmbaIPC_SpinLockIrqSave 1\n");
    /* arrange */
    /* act */
    AmbaIPC_SpinLockIrqSave(SpinID, NULL);

    printf("TEST_AmbaIPC_SpinLockIrqSave 2\n");
    /* arrange */
    SpinID = 0U;
    SpinLockInited = 0U;
    /* act */
    AmbaIPC_SpinLockIrqSave(SpinID, pFlags);

    printf("TEST_AmbaIPC_SpinLockIrqSave 3\n");
    /* arrange */
    SpinID = 0U;
    SpinLockInited = 1U;
    /* act */
    AmbaIPC_SpinLockIrqSave(SpinID, pFlags);

    printf("TEST_AmbaIPC_SpinLockIrqSave done\n");
}

void TEST_AmbaIPC_SpinUnlockIrqRestore(void)
{
    UINT32 SpinID;
    UINT32 Flags = 0U;

    printf("TEST_AmbaIPC_SpinUnlockIrqRestore 1\n");
    /* arrange */
    SpinID = AmbaLinkCtrl.AmbaIpcSpinLockBufSize / sizeof(aspinlock_t); // if (SpinID >= lock_set_size), lock_set_size is set in TEST_AmbaIPC_SpinInit();
    /* act */
    AmbaIPC_SpinUnlockIrqRestore(SpinID, Flags);

    printf("TEST_AmbaIPC_SpinUnlockIrqRestore 2\n");
    /* arrange */
    SpinID = 0U;
    SpinLockInited = 0U;
    /* act */
    AmbaIPC_SpinUnlockIrqRestore(SpinID, Flags);

    printf("TEST_AmbaIPC_SpinUnlockIrqRestore 3\n");
    /* arrange */
    SpinID = 0U;
    SpinLockInited = 1U;
    /* act */
    AmbaIPC_SpinUnlockIrqRestore(SpinID, Flags);

    printf("TEST_AmbaIPC_SpinUnlockIrqRestore done\n");
}

void TEST_AmbaIPC_SpinLock(void)
{
    UINT32 SpinID;

    printf("TEST_AmbaIPC_SpinLock 1\n");
    /* arrange */
    SpinID = AmbaLinkCtrl.AmbaIpcSpinLockBufSize / sizeof(aspinlock_t);
    /* act */
    AmbaIPC_SpinLock(SpinID);

    printf("TEST_AmbaIPC_SpinLock 2\n");
    /* arrange */
    SpinID = 0U;
    SpinLockInited = 0U;
    /* act */
    AmbaIPC_SpinLock(SpinID);

    printf("TEST_AmbaIPC_SpinLock 3\n");
    /* arrange */
    SpinID = 0U;
    SpinLockInited = 1U;
    /* act */
    AmbaIPC_SpinLock(SpinID);

    printf("TEST_AmbaIPC_SpinLock done\n");
}

void TEST_AmbaIPC_SpinUnlock(void)
{
    UINT32 SpinID;

    printf("TEST_AmbaIPC_SpinUnlock 1\n");
    /* arrange */
    SpinID = AmbaLinkCtrl.AmbaIpcSpinLockBufSize / sizeof(aspinlock_t);
    /* act */
    AmbaIPC_SpinUnlock(SpinID);

    printf("TEST_AmbaIPC_SpinUnlock 2\n");
    /* arrange */
    SpinID = 0U;
    SpinLockInited = 0U;
    /* act */
    AmbaIPC_SpinUnlock(SpinID);

    printf("TEST_AmbaIPC_SpinUnlock 3\n");
    /* arrange */
    SpinID = 0U;
    SpinLockInited = 1U;
    /* act */
    AmbaIPC_SpinUnlock(SpinID);

    printf("TEST_AmbaIPC_SpinUnlock done\n");
}

int main(void)
{
    TEST_AmbaIPC_SpinInit();
    TEST_AmbaIPC_SpinLockIrqSave();
    TEST_AmbaIPC_SpinUnlockIrqRestore();
    TEST_AmbaIPC_SpinLock();
    TEST_AmbaIPC_SpinUnlock();
    return 0;
}
