#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaTypes.h"
#include "AmbaCortexA53.h"
#include <unistd.h>
#include <signal.h>

#include "src/AmbaIPC_RPMSG.c"

#define AMBA_COPY     (0U)
#define AMBA_NOCOPY   (1U)
#define AMBA_SAFECOPY (2U)

struct AmbaIPC_VQ_s gVq = {0U};
struct AmbaIPC_RPCLNT_s gRpclnt = {0U};
struct AmbaIPC_RPDEV_s gRpdev = {0U};

AMBA_LINK_CTRL_s AmbaLinkCtrl = {0};
AMBA_LINK_TASK_CTRL_s AmbaLinkTaskCtrl[LINK_TASK_MAX] = {
    [LINK_TASK_INIT] = {
        .TaskName   = "LINK_TASK",
        .Priority   = 115,
        .StackSize  = 0x2000,
        .CoreSel    = 0x1,
    }
};

extern void Set_NoCopy_AmbaWrap_memcpy(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);
extern void Set_NoDummy_IpcPhysToVirt(UINT32 NoDummy);

extern void Set_RetVal_EventFlagCreate(UINT32 RetVal);
extern void Set_RetVal_TaskCreate(UINT32 RetVal);
extern void Set_RetVal_TaskResume(UINT32 RetVal);
extern void Set_RetVal_TaskSetSmpAffinity(UINT32 RetVal);

static UINT32 SetSize_vq_get_avail_buf = 0U;
static UINT32 RetVal_vq_get_avail_buf  = 0U;
static UINT32 RetVal_IO_UtilityStringCompare = 0U;

UINT32 GcovrAlarmFlag;
static void handler(INT32 v)
{
    printf("[Alarm] Timer out!! Force it leave the infinite loop.\n");
    GcovrAlarmFlag = 1U;
}

static void setAlarm(void)
{
    signal(SIGALRM, handler);
    alarm(1);
    printf("[Alarm] Set 1 sec alarm for leaving infinite loop.\n");
}


/****************************************************/
/*                Callback Func                      */
/****************************************************/
void kick(void *data)
{

}

INT32 ipc_msg_handler(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    return 0;
}

/****************************************************/
/*                init Func                         */
/****************************************************/
void init(void)
{
    gVq.kick = kick;
    gRpclnt.svq = &gVq;
    gRpdev.rpclnt = &gRpclnt;
}

/****************************************************/
/*                Local Stub Func                   */
/****************************************************/
void AmbaLink_Free(UINT32 Num, void *pPtr)
{

}

void *AmbaLink_Malloc(UINT32 Num, UINT32 Size)
{

}

void AmbaLink_RpmsgInitDone(void)
{

}

void IO_UtilityStringCopy(char *pDest, SIZE_t DestSize, const char *pSource)
{

}

INT32 IO_UtilityStringCompare(const char *pString1, const char *pString2, SIZE_t Size)
{
    return RetVal_IO_UtilityStringCompare;
}

SIZE_t IO_UtilityStringLength(const char *pString)
{
    return 0U;
}

INT32 AmbaUtility_StringCompare(const char *pString1, const char *pString2, SIZE_t Size)
{
    return 0;
}

SIZE_t AmbaUtility_StringLength(const char *pString)
{
    return 0U;
}

void vq_wait_for_completion(struct AmbaIPC_VQ_s *vq)
{

}

UINT32 vq_init_unused_bufs(struct AmbaIPC_VQ_s *vq, void *buf, UINT32 len)
{
    return 0U;
}

UINT32 vq_kick_prepare(const struct AmbaIPC_VQ_s *vq)
{
    return 0U;
}

void vq_complete(struct AmbaIPC_VQ_s *vq)
{

}

void vq_add_used_buf(struct AmbaIPC_VQ_s *vq, UINT16 idx, UINT32 len)
{

}

INT32 vq_get_avail_buf(struct AmbaIPC_VQ_s *vq, void **buf, UINT32 *len)
{
    if (SetSize_vq_get_avail_buf != 0U) {
        *buf = malloc(SetSize_vq_get_avail_buf);
    }
    return RetVal_vq_get_avail_buf;
}

struct AmbaIPC_VQ_s *vq_create(void (*cb)(struct AmbaIPC_VQ_s *vq),
                               void (*kick)(void *data),
                               UINT16 num,
                               const void *p, UINT32 align, const UINT32 vqid)
{
    return NULL;
}

/****************************************************/
/*                Main Test                         */
/****************************************************/
#if 0
void TEST_(void)
{
    printf("TEST_ 1\n");
    /* arrange */
    /* act */
    ();

    printf("TEST_ done\n");
}
#endif
void TEST_rpdev_alloc(void)
{
    rpdev_cb cb;
    printf("TEST_rpdev_alloc 1\n");
    /* arrange */
    /* act */
    rpdev_alloc(NULL, 0U, cb, NULL);

    printf("TEST_rpdev_alloc done\n");
}

void TEST_rpdev_send(void)
{
    UINT32 Data, Len;

    printf("TEST_rpdev_send 1\n");
    /* arrange */
    /* act */
    rpdev_send(NULL, NULL, 0U);

    printf("TEST_rpdev_send 2\n");
    /* arrange */
    RetVal_vq_get_avail_buf = 1U;
    SetSize_vq_get_avail_buf = sizeof(struct rpmsg_hdr);
    AmbaLinkCtrl.RpmsgBufSize = AMBALINK_UINT32_MAX;
    // setAlarm(); //TMP
    /* act */
    rpdev_send((struct AmbaIPC_RPDEV_s *)&gRpdev, &Data, 0U);
    RetVal_vq_get_avail_buf = 0U;
    SetSize_vq_get_avail_buf = 0U;
    AmbaLinkCtrl.RpmsgBufSize = 0U;

    printf("TEST_rpdev_send 3\n");
    /* arrange */
    RetVal_vq_get_avail_buf = 1U;
    SetSize_vq_get_avail_buf = sizeof(struct rpmsg_hdr);
    AmbaLinkCtrl.RpmsgBufSize = AMBALINK_UINT32_MAX;
    Set_NoCopy_AmbaWrap_memcpy(AMBA_NOCOPY);
    // if (len > (UINT16)AMBALINK_UINT16_MAX)
    Len = AMBALINK_UINT16_MAX+1;
    /* act */
    rpdev_send((struct AmbaIPC_RPDEV_s *)&gRpdev, &Data, Len);
    RetVal_vq_get_avail_buf = 0U;
    SetSize_vq_get_avail_buf = 0U;
    AmbaLinkCtrl.RpmsgBufSize = 0U;
    Set_NoCopy_AmbaWrap_memcpy(AMBA_COPY);

    printf("TEST_rpdev_send done\n");
}

void TEST_rpdev_register(void)
{
    printf("TEST_rpdev_register 1\n");
    /* arrange */
    /* act */
    rpdev_register((struct AmbaIPC_RPDEV_s *)&gRpdev, NULL);

    printf("TEST_rpdev_register 2\n");
    /* arrange */
    gRpdev.dst = RPMSG_NS_ADDR;
    /* act */
    rpdev_register((struct AmbaIPC_RPDEV_s *)&gRpdev, NULL);

    printf("TEST_rpdev_register 3\n");
    /* arrange */
    gRpdev.dst = RPMSG_NS_ADDR;
    RetVal_IO_UtilityStringCompare = 1U;
    /* act */
    rpdev_register((struct AmbaIPC_RPDEV_s *)&gRpdev, NULL);

    printf("TEST_rpdev_register done\n");
}

void TEST_rpdev_unregister(void)
{
    printf("TEST_rpdev_unregister 1\n");
    /* arrange */
    /* act */
    rpdev_unregister(NULL);

    printf("TEST_rpdev_unregister done\n");
}

void TEST_RpmsgCB(void)
{
    printf("TEST_RpmsgCB 1\n");
    /* arrange */
    /* act */
    RpmsgCB((struct AmbaIPC_RPDEV_s *)&gRpdev, NULL, 0U, NULL, 0U);

    printf("TEST_RpmsgCB 2\n");
    /* arrange */
    /* act */
    RpmsgCB((struct AmbaIPC_RPDEV_s *)&gRpdev, NULL, 0U, ipc_msg_handler, 0U);

    printf("TEST_RpmsgCB done\n");
}

void TEST_AmbaIPC_INTEnable(void)
{
    printf("TEST_AmbaIPC_INTEnable 1\n");
    /* arrange */
    /* act */
    AmbaIPC_INTEnable();

    printf("TEST_AmbaIPC_INTEnable done\n");
}

void TEST_AmbaIPC_INTDisable(void)
{
    printf("TEST_AmbaIPC_INTDisable 1\n");
    /* arrange */
    /* act */
    AmbaIPC_INTDisable();

    printf("TEST_AmbaIPC_INTDisable done\n");
}

void TEST_AmbaIPC_UnregisterChannel(void)
{
    AMBA_IPC_HANDLE Channel;
    printf("TEST_AmbaIPC_UnregisterChannel 1\n");
    /* arrange */
    /* act */
    AmbaIPC_UnregisterChannel(Channel);

    printf("TEST_AmbaIPC_UnregisterChannel 2\n");
    /* arrange */
    gRpdev.magicID = RPMSG_MAGIC_ID;
    /* act */
    AmbaIPC_UnregisterChannel(&gRpdev);
    gRpdev.magicID = 0U;

    printf("TEST_AmbaIPC_UnregisterChannel done\n");
}

void TEST_AmbaIPC_TrySend(void)
{
    AMBA_IPC_HANDLE Channel;
    printf("TEST_AmbaIPC_TrySend 1\n");
    /* arrange */
    /* act */
    AmbaIPC_TrySend(Channel, NULL, 0);

    printf("TEST_AmbaIPC_TrySend 2\n");
    /* arrange */
    gRpdev.magicID = RPMSG_MAGIC_ID;
    /* act */
    AmbaIPC_TrySend(&gRpdev, NULL, 1U);
    gRpdev.magicID = 0U;

    printf("TEST_AmbaIPC_TrySend done\n");
}

void TEST_AmbaIPC_RpmsgSuspend(void)
{
    printf("TEST_AmbaIPC_RpmsgSuspend 1\n");
    /* arrange */
    /* act */
    AmbaIPC_RpmsgSuspend();

    printf("TEST_AmbaIPC_RpmsgSuspend done\n");
}

void TEST_AmbaIPC_RpmsgRestoreData(void)
{
    printf("TEST_AmbaIPC_RpmsgRestoreData 1\n");
    /* arrange */
    /* act */
    AmbaIPC_RpmsgRestoreData();

    printf("TEST_AmbaIPC_RpmsgRestoreData done\n");
}

void TEST_AmbaIPC_RpmsgRestoreStatus(void)
{
    printf("TEST_AmbaIPC_RpmsgRestoreStatus 1\n");
    /* arrange */
    /* act */
    AmbaIPC_RpmsgRestoreStatus();

    printf("TEST_AmbaIPC_RpmsgRestoreStatus done\n");
}

void TEST_AmbaIPC_RpmsgRestoreEnable(void)
{
    printf("TEST_AmbaIPC_RpmsgRestoreEnable 1\n");
    /* arrange */
    /* act */
    AmbaIPC_RpmsgRestoreEnable(0U);

    printf("TEST_AmbaIPC_RpmsgRestoreEnable done\n");
}

void TEST_AmbaIPC_RpmsgInit(void)
{
    printf("TEST_AmbaIPC_RpmsgInit 1\n");
    /* arrange */
    Set_RetVal_EventFlagCreate(1U);
    Set_RetVal_TaskCreate(1U);
    Set_RetVal_TaskResume(1U);
    Set_RetVal_TaskSetSmpAffinity(1U);
    /* act */
    AmbaIPC_RpmsgInit();
    Set_RetVal_EventFlagCreate(0U);
    Set_RetVal_TaskCreate(0U);
    Set_RetVal_TaskResume(0U);
    Set_RetVal_TaskSetSmpAffinity(0U);

    printf("TEST_AmbaIPC_RpmsgInit done\n");
}

void TEST_AmbaIPC_dump(void)
{
    printf("TEST_AmbaIPC_dump 1\n");
    /* arrange */
    Set_NoCopy_AmbaWrap_memcpy(AMBA_NOCOPY);
    // Set_NoCopy_AmbaMisra_TypeCast(AMBA_NOCOPY);
    Set_NoDummy_IpcPhysToVirt(1U);
    /* act */
    AmbaIPC_dump();
    Set_NoCopy_AmbaWrap_memcpy(AMBA_COPY);
    // Set_NoCopy_AmbaMisra_TypeCast(AMBA_COPY);
    Set_NoDummy_IpcPhysToVirt(0U);

    printf("TEST_AmbaIPC_dump done\n");
}

int main(void)
{
    init();

    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_();
    // TEST_rpdev_alloc(); //Segmentation fault
    TEST_rpdev_send();
    TEST_rpdev_register();
    TEST_rpdev_unregister();
    TEST_RpmsgCB();
    TEST_AmbaIPC_INTEnable();
    TEST_AmbaIPC_INTDisable();
    TEST_AmbaIPC_UnregisterChannel();
    TEST_AmbaIPC_TrySend();
    TEST_AmbaIPC_RpmsgSuspend();
    TEST_AmbaIPC_RpmsgRestoreData();
    TEST_AmbaIPC_RpmsgRestoreStatus();
    TEST_AmbaIPC_RpmsgRestoreEnable();
    TEST_AmbaIPC_RpmsgInit();
    // TEST_AmbaIPC_dump(); //Segmentation fault
    return 0;
}
