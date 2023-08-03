#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AmbaTypes.h"
#include "amba_osal.h"
#include "AmbaCortexA53.h"
#include "AmbaLinkPrivate.h"

#include "src/AmbaIPC.c"

#define AMBA_COPY     (0U)
#define AMBA_NOCOPY   (1U)
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);

/****************************************************/
/*                init Func                         */
/****************************************************/
static void *initBuf()
{
    void *ptr = malloc(MEM_POOL_IPC_BUF*(UINT32)sizeof(struct AmbaIPC_RPDEV_s));
    return ptr;
}

/****************************************************/
/*                Local Stub Func                   */
/****************************************************/
void AmbaLink_Init(void)
{
}

void RpmsgCB(struct AmbaIPC_RPDEV_s *rpdev, void *data, UINT32 len, void *priv, UINT32 src)
{

}

struct AmbaIPC_RPDEV_s *rpdev_alloc(const char *name, UINT32 flags, rpdev_cb cb, void *priv)
{
    struct AmbaIPC_RPDEV_s *rpdev = NULL;
    return rpdev;
}

INT32 rpdev_register(struct AmbaIPC_RPDEV_s *rpdev, const char *bus_name)
{
    return 0;
}

INT32 rpdev_send(const struct AmbaIPC_RPDEV_s *rpdev, const void *data, UINT32 len)
{
    return 0;
}

static INT32 AmbaIPC_MsgHandler(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    return 0;
}

/****************************************************/
/*                Main Test                         */
/****************************************************/
void TEST_AmbaIPC_Init(void)
{
    printf("TEST_AmbaIPC_Init 1\n");
    /* arrange */
    IPCInited = 0U;
    /* act */
    AmbaIPC_Init();

    printf("TEST_AmbaIPC_Init 2\n");
    /* arrange */
    IPCInited = 1U;
    /* act */
    AmbaIPC_Init();

    printf("TEST_AmbaIPC_Init done\n");
}

void TEST_AmbaIPC_Alloc(void)
{
    UINT32 rpmsgID;
    char *pName;

    printf("TEST_AmbaIPC_Alloc 1\n");
    /* arrange */
    rpmsgID = RPMSG_DEV_AMBA + RPMSG_DEV_OAMP + 1U;
    pName = "test_name";
    /* act */
    AmbaIPC_Alloc(rpmsgID, pName, AmbaIPC_MsgHandler);

    printf("TEST_AmbaIPC_Alloc 2\n");
    /* arrange */
    rpmsgID = RPMSG_DEV_AMBA;
    pName = NULL;
    /* act */
    AmbaIPC_Alloc(rpmsgID, pName, AmbaIPC_MsgHandler);

    printf("TEST_AmbaIPC_Alloc 3\n");
    /* arrange */
    rpmsgID = RPMSG_DEV_AMBA;
    pName = "test_name";
    /* act */
    AmbaIPC_Alloc(rpmsgID, pName, NULL);

    printf("TEST_AmbaIPC_Alloc 4\n");
    /* arrange */
    rpmsgID = RPMSG_DEV_AMBA;
    IPCInited = 0U;
    pName = "test_name";
    /* act */
    AmbaIPC_Alloc(rpmsgID, pName, AmbaIPC_MsgHandler);

    printf("TEST_AmbaIPC_Alloc 5\n");
    /* arrange */
    rpmsgID = RPMSG_DEV_AMBA;
    IPCInited = 1U;
    pName = "test_name";
    /* act */
    AmbaIPC_Alloc(rpmsgID, pName, AmbaIPC_MsgHandler);

    printf("TEST_AmbaIPC_Alloc 6\n");
    /* arrange */
    rpmsgID = RPMSG_DEV_OAMP;
    IPCInited = 1U;
    pName = "test_name";
    /* act */
    AmbaIPC_Alloc(rpmsgID, pName, AmbaIPC_MsgHandler);

    printf("TEST_AmbaIPC_Alloc done\n");
}

void TEST_AmbaIPC_RegisterChannel(void)
{
    const char *pRemote = "test_name";
    AMBA_IPC_HANDLE EchoChannel;
    void * ptrBuf = initBuf();
    struct AmbaIPC_RPDEV_s *rpdev = (struct AmbaIPC_RPDEV_s *)ptrBuf;

    printf("TEST_AmbaIPC_RegisterChannel 1\n");
    /* arrange */
    EchoChannel = NULL;
    /* act */
    AmbaIPC_RegisterChannel(EchoChannel, NULL);

    printf("TEST_AmbaIPC_RegisterChannel 2\n");
    /* arrange */
    EchoChannel = ptrBuf;
    IPCInited = 0U;
    /* act */
    AmbaIPC_RegisterChannel(EchoChannel, NULL);

    printf("TEST_AmbaIPC_RegisterChannel 3\n");
    /* arrange */
    rpdev->magicID = RPMSG_MAGIC_ID;
    EchoChannel = ptrBuf;
    IPCInited = 1U;
    Set_NoCopy_AmbaMisra_TypeCast(AMBA_COPY);
    /* act */
    AmbaIPC_RegisterChannel(EchoChannel, NULL);
    Set_NoCopy_AmbaMisra_TypeCast(AMBA_NOCOPY);

    printf("TEST_AmbaIPC_RegisterChannel 4\n");
    /* arrange */
    rpdev->magicID = RPMSG_MAGIC_ID;
    EchoChannel = ptrBuf;
    IPCInited = 1U;
    Set_NoCopy_AmbaMisra_TypeCast(AMBA_COPY);
    /* act */
    AmbaIPC_RegisterChannel(EchoChannel, pRemote);
    Set_NoCopy_AmbaMisra_TypeCast(AMBA_NOCOPY);

    printf("TEST_AmbaIPC_RegisterChannel done\n");
}

void TEST_AmbaIPC_Send(void)
{
    AMBA_IPC_HANDLE EchoChannel;
    void * msg = "Hello";
    const UINT32 msglen = strlen(msg) + 1;
    void * ptrBuf = initBuf();
    struct AmbaIPC_RPDEV_s *rpdev = (struct AmbaIPC_RPDEV_s *)ptrBuf;
    IPCInited = 1U;

    printf("TEST_AmbaIPC_Send 1\n");
    /* arrange */
    /* act */
    AmbaIPC_Send(NULL, msg, msglen);

    printf("TEST_AmbaIPC_Send 2\n");
    /* arrange */
    EchoChannel = ptrBuf;
    /* act */
    AmbaIPC_Send(EchoChannel, NULL, 1);

    printf("TEST_AmbaIPC_Send 3\n");
    /* arrange */
    EchoChannel = ptrBuf;
    /* act */
    AmbaIPC_Send(EchoChannel, msg, 0);

    printf("TEST_AmbaIPC_Send 4\n");
    /* arrange */
    EchoChannel = ptrBuf;
    IPCInited = 0U;
    /* act */
    AmbaIPC_Send(EchoChannel, msg, msglen);

    printf("TEST_AmbaIPC_Send 5\n");
    /* arrange */
    rpdev->magicID = RPMSG_MAGIC_ID;
    EchoChannel = ptrBuf;
    IPCInited = 1U;
    Set_NoCopy_AmbaMisra_TypeCast(AMBA_COPY);
    /* act */
    AmbaIPC_Send(EchoChannel, msg, msglen);
    Set_NoCopy_AmbaMisra_TypeCast(AMBA_NOCOPY);

    printf("TEST_AmbaIPC_Send 6\n");
    /* arrange */
    rpdev->magicID = RPMSG_MAGIC_ID - 1U;
    EchoChannel = ptrBuf;
    IPCInited = 1U;
    Set_NoCopy_AmbaMisra_TypeCast(AMBA_COPY);
    /* act */
    AmbaIPC_Send(EchoChannel, msg, msglen);
    Set_NoCopy_AmbaMisra_TypeCast(AMBA_NOCOPY);

    printf("TEST_AmbaIPC_Send done\n");
}

int main(void)
{
    TEST_AmbaIPC_Init();
    TEST_AmbaIPC_Alloc();
    TEST_AmbaIPC_RegisterChannel();
    TEST_AmbaIPC_Send();

    /* Dummy call for 100%*/
    RpmsgCB(NULL, NULL, 0U, NULL, 0U);
    AmbaIPC_MsgHandler(NULL, NULL);
    return 0;
}
