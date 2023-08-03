#include "app_base.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_RpcProg_LU_LinuxStreamShare.h"

#define STREAM_NUM (2)

static unsigned char share_buffer[8192] = {0};

static unsigned char stream_enable[STREAM_NUM] = {0};
static CLIENT_ID_t ClientID;

static inline void get_buffer_info(int index, unsigned char **addr, unsigned int *size)
{
    if (index == 0) {
        *addr = share_buffer;
        *size = sizeof(share_buffer) / 4;
    }
    if (index == 1) {
        *addr = share_buffer + sizeof(share_buffer) / 4;
        *size = sizeof(share_buffer) / 2;
    }
}

static int stream_init(void)
{
    static int inited = 0;

    if (inited) {
        return 0;
    }
    ClientID = AmbaIPC_ClientCreate((int)AMBA_IPC_HOST_LINUX,
                                     (int)AMBA_RPC_PROG_LU_STREAM_SHARE_PROG_ID,
                                     (int)AMBA_RPC_PROG_LU_STREAM_SHARE_VER);
    if (ClientID == NULL) {
        debug_line("[%s] AmbaIPC_ClientCreate failed", __func__);
        return -1;
    }
    inited = 1;

    return 0;
}

int stream_share_get_info(unsigned long long *share_addr, unsigned long long *phy_addr, unsigned int *share_size)
{
    UINT32 _phy_addr = 0, _share_addr = 0;

    _share_addr = (unsigned int)&share_buffer;
    if (AmbaMMU_Virt32ToPhys32(_share_addr, &_phy_addr) == MMU_ERR_NONE) {
        *share_addr = CAST_TO_UINTPRT(_share_addr);
        *phy_addr = (unsigned long long)_phy_addr;
        *share_size = sizeof(share_buffer);
        return 0;
    }

    return -1;
}

int stream_share_send_data(int index, unsigned char *data, unsigned int len, int extra_param)
{
    AMBA_IPC_REPLY_STATUS_e Status;
    stream_share_param_s param;
    unsigned int out;
    unsigned char *tmp_addr = NULL;
    unsigned int tmp_size = 0;

    if (stream_enable[index] == 0) {
        return -1;
    }
    if (stream_init() < 0) {
        return -1;
    }
    get_buffer_info(index, &tmp_addr, &tmp_size);
    memcpy(tmp_addr, data, len);
    param.eos = 0;
    param.size = len;
    param.extra_param = extra_param;
    (void)AmbaCache_DataClean((unsigned int)tmp_addr, tmp_size);
    Status = AmbaIPC_ClientCall(ClientID, (int)(index + 1),
                                &param, sizeof(stream_share_param_s),
                                &out, sizeof(out), 0);
    if (Status != AMBA_IPC_REPLY_SUCCESS) {
        debug_line("[%s] AmbaIPC_ClientCall failed: %d", __func__, Status);
        return -1;
    }

    return 0;
}

int stream_share_set_enable(int index, int enable)
{
    stream_enable[index] = enable;

    return 0;
}

