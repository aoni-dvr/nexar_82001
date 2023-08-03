#include "app_base.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_RpcProg_LU_LinuxBtStream.h"

static unsigned char imu_buffer[256] = {0};
static unsigned char gnss_buffer[256] = {0};
static CLIENT_ID_t ClientID;
static int imu_stream_enable = 0, gnss_stream_enable = 0;
static unsigned char imu_message_seq_num = 0;

static int imu_stream_send_notify_impl(unsigned char *data, unsigned int len, unsigned char eos);
static int gnss_stream_send_notify_impl(unsigned char *data, unsigned int len, unsigned char eos);

static void stream_share_bt_imu_stream_notify_timer_handler(int eid)
{
    static int cnt = 0;
    static unsigned char seq_num_1s = 1, seq_num_10s = 1;
    TEMP_DATA_s data = {0};
    unsigned char buffer[10] = {0};

    if (eid == TIMER_UNREGISTER) {
        seq_num_1s = 1;
        seq_num_10s = 1;
        cnt = 0;
        return;
    }
    cnt++;
    if (cnt % 10 == 0) {
        cnt = 0;
        //send start time
        seq_num_10s++;
        buffer[0] = 0x00;
        buffer[1] = seq_num_10s;
        imu_stream_send_notify_impl(buffer, 10, 0);
        if (seq_num_10s >= 255) {
            seq_num_10s = 0;
        }
    }
    //send imu temperature
    seq_num_1s++;
    buffer[0] = 0x01;
    buffer[1] = seq_num_1s;
    Imu_GetTemperature(&data);
    buffer[6] = (data.temp_float16 >> 8) & 0xFF;
    buffer[7] = data.temp_float16 & 0xFF;
    imu_stream_send_notify_impl(buffer, 8, 0);
    if (seq_num_1s >= 255) {
        seq_num_1s = 0;
    }
}

int bt_stream_share_set_imu_stream_enable(int enable)
{
    if (imu_stream_enable == 0 && enable == 1) {
        //connected
        unsigned char buffer[10] = {0};
        TEMP_DATA_s data = {0};
        buffer[0] = 0x00;
        buffer[1] = 0x00;
        imu_stream_send_notify_impl(buffer, 10, 0);
        buffer[0] = 0x01;
        buffer[1] = 0x01;
        Imu_GetTemperature(&data);
        buffer[6] = (data.temp_float16 >> 8) & 0xFF;
        buffer[7] = data.temp_float16 & 0xFF;
        imu_stream_send_notify_impl(buffer, 8, 0);
        app_timer_register(TIMER_1HZ, stream_share_bt_imu_stream_notify_timer_handler);
    } else if (imu_stream_enable == 1 && enable == 0) {
        //disconnected
        app_timer_unregister(TIMER_1HZ, stream_share_bt_imu_stream_notify_timer_handler);
    }
    imu_message_seq_num = 0;
    imu_stream_enable = enable;

    return 0;
}

static void stream_share_bt_gnss_stream_notify_timer_handler(int eid)
{
    static int cnt = 0;
    unsigned char buffer[10] = {0};
    static unsigned char seq_num_10s = 1;

    if (eid == TIMER_UNREGISTER) {
        seq_num_10s = 1;
        cnt = 0;
        return;
    }
    cnt++;
    if (cnt % 10 == 0) {
        cnt = 0;
        //send start time
        seq_num_10s++;
        buffer[0] = 0x10;
        buffer[1] = seq_num_10s;
        gnss_stream_send_notify_impl(buffer, 10, 0);
        if (seq_num_10s >= 255) {
            seq_num_10s = 0;
        }
    }
}

int bt_stream_share_set_gnss_stream_enable(int enable)
{
    if (gnss_stream_enable == 0 && enable == 1) {
        //connected
        unsigned char buffer[10] = {0};
        buffer[0] = 0x10;
        buffer[1] = 0x00;
        gnss_stream_send_notify_impl(buffer, 10, 0);
        app_timer_register(TIMER_1HZ, stream_share_bt_gnss_stream_notify_timer_handler);
    } else if (gnss_stream_enable == 1 && enable == 0) {
        //disconnected
        app_timer_unregister(TIMER_1HZ, stream_share_bt_gnss_stream_notify_timer_handler);
    }
    gnss_stream_enable = enable;

    return 0;
}

int bt_imu_stream_get_share_info(unsigned long long *share_addr, unsigned long long *phy_addr, unsigned int *share_size)
{
    UINT32 _phy_addr = 0, _share_addr = 0;

    _share_addr = (UINT32)&imu_buffer[0];
    if (AmbaMMU_Virt32ToPhys32(_share_addr, &_phy_addr) == MMU_ERR_NONE) {
        *share_addr = CAST_TO_UINTPRT(_share_addr);
        *phy_addr = (UINT64)_phy_addr;
        *share_size = sizeof(imu_buffer);
        return 0;
    }
    return -1;
}

int bt_gnss_stream_get_share_info(unsigned long long *share_addr, unsigned long long *phy_addr, unsigned int *share_size)
{
    UINT32 _phy_addr = 0, _share_addr = 0;

    _share_addr = (UINT32)&gnss_buffer[0];
    if (AmbaMMU_Virt32ToPhys32(_share_addr, &_phy_addr) == MMU_ERR_NONE) {
        *share_addr = CAST_TO_UINTPRT(_share_addr);
        *phy_addr = (UINT64)_phy_addr;
        *share_size = sizeof(gnss_buffer);
        return 0;
    }
    return -1;
}

static int stream_init(void)
{
    static int inited = 0;

    if (inited) {
        return 0;
    }
    ClientID = AmbaIPC_ClientCreate((INT32)AMBA_IPC_HOST_LINUX,
                                         (INT32)AMBA_RPC_PROG_LU_BT_STREAM_PROG_ID,
                                         (INT32)AMBA_RPC_PROG_LU_BT_STREAM_VER);
    if (ClientID == NULL) {
        debug_line("[%s] AmbaIPC_ClientCreate failed", __func__);
        return -1;
    }
    inited = 1;

    return 0;
}

static int imu_stream_send_notify_impl(unsigned char *data, unsigned int len, unsigned char eos)
{
    bt_stream_param_s param;
    unsigned int out;
    AMBA_IPC_REPLY_STATUS_e Status;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    if (stream_init() < 0) {
        return -1;
    }
    SvcUserPref_Get(&pSvcUserPref);
    param.size = len;
    param.eos = eos;
    param.timezone = pSvcUserPref->TimeZone;
    memcpy(imu_buffer, data, len);
    (void)AmbaCache_DataClean((UINT32)imu_buffer, sizeof(imu_buffer));
    Status = AmbaIPC_ClientCall(ClientID, (INT32)LU_BT_STREAM_FUNC_IMU_NOTIFY,
                                &param, sizeof(bt_stream_param_s),
                                &out, sizeof(out), 0);
    if (Status != AMBA_IPC_REPLY_SUCCESS) {
        debug_line("[%s] AmbaIPC_ClientCall failed: %d", __func__, Status);
        return -1;
    }

    return 0;
}

static int gnss_stream_send_notify_impl(unsigned char *data, unsigned int len, unsigned char eos)
{
    bt_stream_param_s param;
    unsigned int out;
    AMBA_IPC_REPLY_STATUS_e Status;
    SVC_USER_PREF_s *pSvcUserPref = NULL;

    if (stream_init() < 0) {
        return -1;
    }
    SvcUserPref_Get(&pSvcUserPref);
    param.size = len;
    param.eos = eos;
    param.timezone = pSvcUserPref->TimeZone;
    memcpy(gnss_buffer, data, len);
    (void)AmbaCache_DataClean((UINT32)gnss_buffer, sizeof(gnss_buffer));
    Status = AmbaIPC_ClientCall(ClientID, (INT32)LU_BT_STREAM_FUNC_GNSS_NOTIFY,
                                &param, sizeof(bt_stream_param_s),
                                &out, sizeof(out), 0);
    if (Status != AMBA_IPC_REPLY_SUCCESS) {
        debug_line("[%s] AmbaIPC_ClientCall failed: %d", __func__, Status);
        return -1;
    }

    return 0;
}

int bt_imu_stream_check_enable(void)
{
    return imu_stream_enable;
}

int bt_imu_stream_send_message(unsigned short acc_x, unsigned short acc_y, unsigned short acc_z,
                                unsigned short gyro_x, unsigned short gyro_y, unsigned short gyro_z)
{
    unsigned char buffer[18] = {0};

    imu_message_seq_num++;
    buffer[0] = 0x02;
    buffer[1] = imu_message_seq_num;

    buffer[6] = (acc_x >> 8) & 0xFF;
    buffer[7] = acc_x & 0xFF;
    buffer[8] = (acc_y >> 8) & 0xFF;
    buffer[9] = acc_y & 0xFF;
    buffer[10] = (acc_z >> 8) & 0xFF;
    buffer[11] = acc_z & 0xFF;

    buffer[12] = (gyro_x >> 8) & 0xFF;
    buffer[13] = gyro_x & 0xFF;
    buffer[14] = (gyro_y >> 8) & 0xFF;
    buffer[15] = gyro_y & 0xFF;
    buffer[16] = (gyro_z >> 8) & 0xFF;
    buffer[17] = gyro_z & 0xFF;
    if (imu_message_seq_num >= 255) {
        imu_message_seq_num = 0;
    }
    return imu_stream_send_notify_impl(buffer, 18, 0);
}

int bt_gnss_stream_check_enable(void)
{
    return gnss_stream_enable;
}

