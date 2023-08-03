#include "app_base.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_RpcProg_LU_RtosLogShare.h"

static unsigned char share_buffer[4096] = {0};

static int stream_enable = 0;
static CLIENT_ID_t ClientID;

static inline void get_buffer_info(unsigned char **addr, unsigned int *size)
{
    *addr = share_buffer;
    *size = sizeof(share_buffer);
}

static int stream_init(void)
{
    static int inited = 0;

    if (inited) {
        return 0;
    }
    ClientID = AmbaIPC_ClientCreate((int)AMBA_IPC_HOST_LINUX,
                                     (int)AMBA_RPC_PROG_LU_RTOS_LOG_PROG_ID,
                                     (int)AMBA_RPC_PROG_LU_RTOS_LOG_VER);
    if (ClientID == NULL) {
        debug_line("[%s] AmbaIPC_ClientCreate failed", __func__);
        return -1;
    }
    inited = 1;

    return 0;
}

int rtos_log_share_get_info(unsigned long long *share_addr, unsigned long long *phy_addr, unsigned int *share_size)
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

#if 0
void rtos_log_share_send_power_on_log(void)
{
    unsigned char *buf = NULL;
    unsigned int len = 0;

    if (stream_enable & 0x80) {
        stream_enable &= ~0x80;
        offline_log_get_mutex();
        buf = offline_log_get_power_on_log_buffer(&len);
        if (len > 0) {
            int once_len = sizeof(share_buffer) - 1;
            int times = len / once_len;
            int remain_size = len % once_len;
            if (times > 0) {
                int i = 0;
                for (i = 0; i < times; i++) {
                    rtos_log_share_send_data(buf + i * once_len, once_len);
                    msleep(50);
                }
            }
            if (remain_size > 0) {
                rtos_log_share_send_data(buf + times * once_len, remain_size);
            }
        }
        offline_log_release_mutex();
    }
}
#endif

int rtos_log_share_send_data(const unsigned char *data, unsigned int len)
{
    AMBA_IPC_REPLY_STATUS_e Status;
    unsigned int out;
    unsigned char *tmp_addr = NULL;
    unsigned int tmp_size = 0, count = 0, i = 0;

    if (stream_enable == 0 || len <= 0) {
        return -1;
    }
    if (stream_init() < 0) {
        return -1;
    }
    get_buffer_info(&tmp_addr, &tmp_size);
    count = len / tmp_size;
    if (len % tmp_size != 0) {
        count += 1;
    }
    for (i = 0; i < count; i++) {
        unsigned int tmp_len = (len - i * tmp_size) >= tmp_size ? tmp_size : len % tmp_size;
        memset(tmp_addr, 0, tmp_size);
        memcpy(tmp_addr, data + i * tmp_size, tmp_len);
        (void)AmbaCache_DataClean((unsigned int)tmp_addr, tmp_size);
        Status = AmbaIPC_ClientCall(ClientID, (int)(1),
                                    &tmp_len, sizeof(tmp_len),
                                    &out, sizeof(out), 5000);
        if (Status != AMBA_IPC_REPLY_SUCCESS) {
            debug_line("[%s] AmbaIPC_ClientCall failed: %d", __func__, Status);
            return -1;
        }
    }

    return 0;
}

int rtos_log_share_set_enable(int enable)
{
    stream_enable = enable;

    return 0;
}

