#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaTypes.h"
#include "AmbaINT_Def.h"
#include "AmbaWrap.h"
#include "amba_osal.h"
#include "AmbaIPC.h"

#include "AmbaCortexA53.h"
#include "src/AmbaIPC_Mutex.c"

#define AMBA_COPY     (0U)
#define AMBA_NOCOPY   (1U)
#define AMBA_SAFECOPY (2U)

amutex_share_t g_share = { .slock = 1U, .owner = 0U, .wait_list = 1U };

static UINT32 kal_event_flag_get = 0U;
static UINT32 gcovr_init_flag = 0U;

AMBA_LINK_CTRL_s AmbaLinkCtrl = { .AmbaLinkMachineRev = 1U };

extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_NoCopy_AmbaWrap_memset(UINT32 NoCopy);
extern void Set_NoCopy_AmbaWrap_memcpy(UINT32 NoCopy);
extern void Set_RetVal_EventFlagGet(UINT32 RetVal);
extern void (*fpCB_EventFlagGet)(void);

void CB_EventFlagGet(void)
{
    g_share.owner = 0U;
}

/****************************************************/
/*                init Func                         */
/****************************************************/
static void init(void)
{
    (void)memset(&lock_set, 0, sizeof(lock_set));
    lock_set.share = (amutex_share_t *) &g_share;
    fpCB_EventFlagGet = CB_EventFlagGet;
}

void gcovr_init_share(void)
{
    if (gcovr_init_flag == 0U) {
        g_share.slock = 1U;
        g_share.owner = 2U;
        g_share.wait_list = 1U;
    } else if (gcovr_init_flag == 1U) {
        g_share.slock = 1U;
        g_share.owner = CORE_LOCAL;
        g_share.wait_list = 0U;
    } else if (gcovr_init_flag == 2U) {
        g_share.slock = 1U;
        g_share.owner = CORE_LOCAL;
        g_share.wait_list = 1U;
    }
}

/****************************************************/
/*                Local Stub Func                   */
/****************************************************/
UINT32 do_SpinLockIrqSave(UINT32 *lock, UINT32 *pFlags)
{
    return 0;
}

UINT32 do_SpinUnlockIrqRestore(UINT32 *lock, UINT32 Flags)
{
    return 0;
}

UINT32 AmbaRTSL_GicIntConfig(UINT32 IntID, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg)
{
    return 0;
}

UINT32 AmbaRTSL_GicIntEnable(UINT32 IntID)
{
    return 0;
}

/****************************************************/
/*                Main Test                         */
/****************************************************/

void TEST_AmbaIPC_MutexTake(void)
{
    UINT32 MutexID, Timeout;
    Timeout = 0U;

    printf("TEST_AmbaIPC_MutexTake 1\n");
    /* arrange */
    MutexID = AMBA_IPC_NUM_MUTEX;
    /* act */
    AmbaIPC_MutexTake(MutexID, Timeout);

    printf("TEST_AmbaIPC_MutexTake 2\n");
    /* arrange */
    MutexInited = 0U;
    MutexID = AMBA_IPC_NUM_MUTEX - 1U;
    /* act */
    AmbaIPC_MutexTake(MutexID, Timeout);

    printf("TEST_AmbaIPC_MutexTake 3\n");
    /* arrange */
    MutexInited = 1U;
    Set_RetVal_MutexTake(1U);
    MutexID = 0U;
    /* act */
    AmbaIPC_MutexTake(MutexID, Timeout);
    Set_RetVal_MutexTake(0U);

    printf("TEST_AmbaIPC_MutexTake 4\n");
    /* arrange */
    MutexInited = 1U;
    MutexID = 0U;
    gcovr_init_flag = 0U;
    Set_RetVal_EventFlagGet(1U);
    gcovr_init_share();
    /* act */
    AmbaIPC_MutexTake(MutexID, Timeout);
    Set_RetVal_EventFlagGet(0U);

    printf("TEST_AmbaIPC_MutexTake 5\n");
    /* arrange */
    MutexInited = 1U;
    MutexID = 0U;
    gcovr_init_flag = 1U;
    gcovr_init_share();
    /* act */
    AmbaIPC_MutexTake(MutexID, Timeout);

    printf("TEST_AmbaIPC_MutexTake done\n");
}

void TEST_AmbaIPC_MutexGive(void)
{
    UINT32 MutexID;
    UINT32 i;

    printf("TEST_AmbaIPC_MutexGive 1\n");
    /* arrange */
    MutexID = AMBA_IPC_NUM_MUTEX;
    /* act */
    AmbaIPC_MutexGive(MutexID);

    printf("TEST_AmbaIPC_MutexGive 2\n");
    /* arrange */
    MutexInited = 0U;
    MutexID = AMBA_IPC_NUM_MUTEX - 1U;
    /* act */
    AmbaIPC_MutexGive(MutexID);

    printf("TEST_AmbaIPC_MutexGive 3\n");
    /* arrange */
    MutexInited = 1U;
    Set_RetVal_MutexGive(1U);
    MutexID = 0U;
    gcovr_init_flag = 0U;
    gcovr_init_share();
    /* act */
    AmbaIPC_MutexGive(MutexID);

    printf("TEST_AmbaIPC_MutexGive 4\n");
    /* arrange */
    MutexInited = 1U;
    Set_RetVal_MutexGive(1U);
    MutexID = 0U;
    gcovr_init_flag = 2U;
    gcovr_init_share();
    lock_set.local[MutexID].count = 1U;
    Set_NoCopy_AmbaWrap_memcpy(AMBA_SAFECOPY);
    /* act */
    AmbaIPC_MutexGive(MutexID);
    Set_NoCopy_AmbaWrap_memcpy(AMBA_COPY);

    printf("TEST_AmbaIPC_MutexGive done\n");
}

void TEST_AmbaIPC_MutexInit(void)
{
    printf("TEST_AmbaIPC_MutexInit 1\n");
    /* arrange */
    /* act */
    AmbaIPC_MutexInit();

    printf("TEST_AmbaIPC_MutexInit 2\n");
    /* arrange */
    AmbaLinkCtrl.AmbaIpcMutexBufSize = sizeof(amutex_share_t) * AMBA_IPC_NUM_MUTEX + 1U;
    Set_NoCopy_AmbaWrap_memset(AMBA_NOCOPY);
    /* act */
    AmbaIPC_MutexInit();
    Set_NoCopy_AmbaWrap_memset(AMBA_COPY);

    printf("TEST_AmbaIPC_MutexInit done\n");
}

void TEST_AmbaIPC_OWNER_IS_LOCAL(void)
{
    UINT32 id;

    printf("TEST_AmbaIPC_OWNER_IS_LOCAL 1\n");
    /* arrange */
    id = 0;
    /* act */
    AmbaIPC_OWNER_IS_LOCAL(id);

    printf("TEST_AmbaIPC_OWNER_IS_LOCAL 2\n");
    /* arrange */
    id = AMBA_IPC_NUM_MUTEX;
    /* act */
    AmbaIPC_OWNER_IS_LOCAL(id);

    printf("TEST_AmbaIPC_OWNER_IS_LOCAL done\n");
}

void TEST_AipcMutexIsr(void)
{
    UINT32 IntID;

    printf("TEST_AipcMutexIsr 1\n");
    /* arrange */
    IntID = 0;
    /* act */
    AipcMutexIsr(IntID, 0U);

    printf("TEST_AipcMutexIsr 2\n");
    /* arrange */
    IntID = AXI_SOFT_IRQ0;
    /* act */
    Set_NoCopy_AmbaWrap_memcpy(AMBA_SAFECOPY);
    AipcMutexIsr(IntID, 0U);
    Set_NoCopy_AmbaWrap_memcpy(AMBA_COPY);

    printf("TEST_AipcMutexIsr done\n");
}

int main(void)
{
    init();
    TEST_AmbaIPC_MutexTake();
    TEST_AmbaIPC_MutexGive();
    TEST_AmbaIPC_OWNER_IS_LOCAL();
    TEST_AipcMutexIsr();
    TEST_AmbaIPC_MutexInit(); // must in the last
    return 0;
}
