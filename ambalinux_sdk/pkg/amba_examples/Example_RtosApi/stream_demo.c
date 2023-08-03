#include "rtos_api_lib.h"
#include "debug.h"
#include "aipc_user.h"
#include "AmbaIPC_RpcProg_LU_LinuxStreamShare.h"
#include <signal.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <pthread.h>
#include <syslog.h>

#define STREAM_NUM (2)

typedef struct _stream_s_ {
    unsigned char *ptr;
    mmapInfo_s map;
    stream_share_info_s share_info;
} stream_s;

static stream_s streams[1] = {0};

//stream ptr:
//nmea: ptr
//mic: ptr + 2048
static void nmea_stream_notify_cb(stream_share_param_s *param, AMBA_IPC_SVC_RESULT_s *pRet)
{
    int ret = 0;
    int *i_ptr;

    debug_line("nmea data size: %d, eos: %d, %s", param->size, param->eos, (char *)(streams[0].ptr));

    i_ptr = (int *)(pRet->pResult);
    *i_ptr = ret;
    //send ipc reply
    pRet->Length = sizeof(int);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

static void mic_stream_notify_cb(stream_share_param_s *param, AMBA_IPC_SVC_RESULT_s *pRet)
{
    int ret = 0;
    int *i_ptr;

    //debug_line("mic data id: %d, size: %d, eos: %d", param->extra_param, param->size, param->eos);

    i_ptr = (int *)(pRet->pResult);
    *i_ptr = ret;
    //send ipc reply
    pRet->Length = sizeof(int);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

int stream_demo_init(void)
{
    AMBA_IPC_PROG_INFO_s prog_info;
    int ret = 0;
    int func_slot = 0;
    int i = 0;

    prog_info.ProcNum = LU_STREAM_SHARE_FUNC_AMOUNT - 1;
    prog_info.pProcInfo = malloc(prog_info.ProcNum * sizeof(AMBA_IPC_PROC_s));
    func_slot = LU_STREAM_SHARE_FUNC_NMEA_NOTIFY - 1;
    prog_info.pProcInfo[func_slot].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[func_slot].Proc = (AMBA_IPC_PROC_f)&nmea_stream_notify_cb;
    func_slot = LU_STREAM_SHARE_FUNC_MIC_NOTIFY - 1;
    prog_info.pProcInfo[func_slot].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[func_slot].Proc = (AMBA_IPC_PROC_f)&mic_stream_notify_cb;
    ret = ambaipc_svc_register(AMBA_RPC_PROG_LU_STREAM_SHARE_PROG_ID, AMBA_RPC_PROG_LU_STREAM_SHARE_VER, "stream_share", &prog_info, 1);
    debug_line("ambaipc_svc_register ret=%d", ret);
    if (ret != 0) {
        free(prog_info.pProcInfo);
        return -1;
    }
    free(prog_info.pProcInfo);

    streams[0].map.base = NULL;
    rtos_api_lib_get_stream_share_info(&streams[0].share_info);
    streams[0].ptr = (unsigned char *)rtos_api_lib_convert_memory2linux(streams[0].share_info, &streams[0].map);
    if (streams[0].ptr != NULL) {
        for (i = 0; i < STREAM_NUM; i++) {
            rtos_api_lib_set_stream_share_enable(i, 1);
        }
    }

    return 0;
}

int stream_demo_deinit(void)
{
    int i = 0;
    for (i = 0; i < STREAM_NUM; i++) {
        rtos_api_lib_set_stream_share_enable(i, 0);
    }
    rtos_api_lib_munmap(&streams[0].map);
    ambaipc_svc_unregister(AMBA_RPC_PROG_LU_STREAM_SHARE_PROG_ID, AMBA_RPC_PROG_LU_STREAM_SHARE_VER);

    return 0;
}

static void signal_handler(int signal)
{
    static int bExit = 0;
    if (bExit == 0) {
        bExit = 1;
        debug_line("streamn_demo exit by signal:%d\n", signal);
        stream_demo_deinit();
        exit(1);
    }
}

static void capture_all_signal()
{
    int i = 0;
    for(i = 0; i < 32; i ++) {
        if ( (i == SIGPIPE) || (i == SIGCHLD) || (i == SIGALRM) || (i == SIGPROF)) {
            signal(i, SIG_IGN);
        } else {
            signal(i, signal_handler);
        }
    }
}

static sem_t main_sem;
int main(int argc, char **argv)
{
    capture_all_signal();

    if (stream_demo_init() < 0) {
        stream_demo_deinit();
        return -1;
    }
    //wait
    sem_init(&main_sem, 0, 0);
    sem_wait(&main_sem);
    sem_destroy(&main_sem);

    return 0;
}

