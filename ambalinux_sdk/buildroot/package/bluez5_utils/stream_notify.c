#include "rtos_api_lib.h"
#include "aipc_user.h"
#include "AmbaIPC_RpcProg_LU_LinuxBtStream.h"

static unsigned char *imu_stream_ptr = NULL;
static mmapInfo_s imu_stream_map = {0};

static unsigned char *gnss_stream_ptr = NULL;
static mmapInfo_s gnss_stream_map = {0};

static void imu_stream_notify_cb(bt_stream_param_s *param, AMBA_IPC_SVC_RESULT_s *pRet)
{
    int ret = 0;
    int *i_ptr;

    if (param == NULL || pRet == NULL) {
        return;
    }
    //syslog(LOG_INFO, "#%s data size:%d eos: %d\n", __func__, info->size, info->eos);
    if (param->eos == 0 && param->size > 0) {
        unsigned long long cur_time = 0;
        static unsigned long long start_time = 0;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        cur_time = (tv.tv_sec + param->timezone) * 10000 + tv.tv_usec / 100;
        if (*imu_stream_ptr == 0x00) {//start time frame
            if (*(imu_stream_ptr + 1) == 0x00) {
                *(imu_stream_ptr + 1) = 0x01;
                start_time = cur_time;
            }
            //fill start time
            for (int i = 0; i < 8; i++) {
                *(imu_stream_ptr + 2 + i) = (start_time >> (56 - i * 8)) & 0xFF;
            }
        } else {
            //fill diff time
            unsigned int diff_time = cur_time - start_time;
            for (int i = 0; i < 4; i++) {
                *(imu_stream_ptr + 2 + i) = (diff_time >> (24 - i * 8)) & 0xFF;
            }
        }
        ret = send_imu_notification(imu_stream_ptr, param->size);
    }
    //prepare RPC result
    i_ptr = (int *)(pRet->pResult);
    *i_ptr = ret;
    //send ipc reply
    pRet->Length = sizeof(int);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

static void gnss_stream_notify_cb(bt_stream_param_s *param, AMBA_IPC_SVC_RESULT_s *pRet)
{
    int ret = 0;
    int *i_ptr;

    if (param == NULL || pRet == NULL) {
        return;
    }
    //syslog(LOG_INFO, "#%s data size:%d eos: %d\n", __func__, info->size, info->eos);
    if (param->eos == 0 && param->size > 0) {
        unsigned long long cur_time = 0;
        static unsigned long long start_time = 0;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        cur_time = (tv.tv_sec + param->timezone) * 10000 + tv.tv_usec / 100;
        if (*gnss_stream_ptr == 0x10) {//start time frame
            if (*(gnss_stream_ptr + 1) == 0x00) {
                *(gnss_stream_ptr + 1) = 0x01;
                start_time = cur_time;
            }
            //fill start time
            for (int i = 0; i < 8; i++) {
                *(gnss_stream_ptr + 2 + i) = (start_time >> (56 - i * 8)) & 0xFF;
            }
        } else {
            //fill diff time
            unsigned int diff_time = cur_time - start_time;
            for (int i = 0; i < 4; i++) {
                *(gnss_stream_ptr + 2 + i) = (diff_time >> (24 - i * 8)) & 0xFF;
            }
        }
        ret = send_gnss_notification(gnss_stream_ptr, param->size);
    }
    //prepare RPC result
    i_ptr = (int *)(pRet->pResult);
    *i_ptr = ret;
    //send ipc reply
    pRet->Length = sizeof(int);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

int stream_notify_init(void)
{
    AMBA_IPC_PROG_INFO_s prog_info;
    int ret = 0;
    int func_slot = 0;
    stream_share_info_s stream_info;

    imu_stream_map.base = NULL;
    gnss_stream_map.base = NULL;

    prog_info.ProcNum = LU_BT_STREAM_FUNC_AMOUNT - 1;
    prog_info.pProcInfo = malloc(prog_info.ProcNum * sizeof(AMBA_IPC_PROC_s));
    func_slot = LU_BT_STREAM_FUNC_IMU_NOTIFY - 1;
    prog_info.pProcInfo[func_slot].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[func_slot].Proc = (AMBA_IPC_PROC_f)&imu_stream_notify_cb;
    func_slot = LU_BT_STREAM_FUNC_GNSS_NOTIFY - 1;
    prog_info.pProcInfo[func_slot].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[func_slot].Proc = (AMBA_IPC_PROC_f)&gnss_stream_notify_cb;
    ret = ambaipc_svc_register(AMBA_RPC_PROG_LU_BT_STREAM_PROG_ID, AMBA_RPC_PROG_LU_BT_STREAM_VER, "bt_stream", &prog_info, 1);
    if (ret != 0) {
        free(prog_info.pProcInfo);
        return -1;
    }
    free(prog_info.pProcInfo);

    if (imu_stream_ptr == NULL) {
        rtos_api_lib_get_bt_imu_stream_info(&stream_info);
        imu_stream_ptr = (unsigned char *)rtos_api_lib_convert_memory2linux(stream_info, &imu_stream_map);
    }
    if (imu_stream_ptr == NULL) {
        return -1;
    }
    if (gnss_stream_ptr == NULL) {
        rtos_api_lib_get_bt_gnss_stream_info(&stream_info);
        gnss_stream_ptr = (unsigned char *)rtos_api_lib_convert_memory2linux(stream_info, &gnss_stream_map);
    }
    if (gnss_stream_ptr == NULL) {
        return -1;
    }

    return 0;
}

int stream_notify_deinit(void)
{
    rtos_api_lib_set_bt_imu_stream_enable(0);
    rtos_api_lib_set_bt_gnss_stream_enable(0);
    ambaipc_svc_unregister(AMBA_RPC_PROG_LU_BT_STREAM_PROG_ID, AMBA_RPC_PROG_LU_BT_STREAM_VER);
    rtos_api_lib_munmap(&imu_stream_map);
    imu_stream_ptr = NULL;
    rtos_api_lib_munmap(&gnss_stream_map);
    gnss_stream_ptr = NULL;

    return 0;
}

