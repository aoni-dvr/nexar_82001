#include "rtos_api_lib.h"
#include "debug.h"
#include "aipc_user.h"
#include "AmbaIPC_RpcProg_LU_RtosLogShare.h"
#include <signal.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <pthread.h>
#include <syslog.h>

typedef struct _stream_s_ {
    unsigned char *ptr;
    mmapInfo_s map;
    stream_share_info_s share_info;
} stream_s;

static stream_s stream = {0};

static void rtos_log_stream_notify_cb(void *param, AMBA_IPC_SVC_RESULT_s *pRet)
{
    int ret = 0;
    int *i_ptr;
    int len = *((int *)param);

    printf("%.*s", len, stream.ptr);

    i_ptr = (int *)(pRet->pResult);
    *i_ptr = ret;
    //send ipc reply
    pRet->Length = sizeof(int);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

int rtos_log_init(void)
{
    AMBA_IPC_PROG_INFO_s prog_info;
    int ret = 0;
    int func_slot = 0;

    prog_info.ProcNum = LU_RTOS_LOG_SHARE_FUNC_AMOUNT - 1;
    prog_info.pProcInfo = malloc(prog_info.ProcNum * sizeof(AMBA_IPC_PROC_s));
    func_slot = LU_RTOS_LOG_SHARE_FUNC_NOTIFY - 1;
    prog_info.pProcInfo[func_slot].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[func_slot].Proc = (AMBA_IPC_PROC_f)&rtos_log_stream_notify_cb;
    ret = ambaipc_svc_register(AMBA_RPC_PROG_LU_RTOS_LOG_PROG_ID, AMBA_RPC_PROG_LU_RTOS_LOG_VER, "rtos_log", &prog_info, 1);
    debug_line("ambaipc_svc_register ret=%d", ret);
    if (ret != 0) {
        free(prog_info.pProcInfo);
        return -1;
    }
    free(prog_info.pProcInfo);

    stream.map.base = NULL;
    rtos_api_lib_get_rtos_log_share_info(&stream.share_info);
    stream.ptr = (unsigned char *)rtos_api_lib_convert_memory2linux(stream.share_info, &stream.map);
    if (stream.ptr != NULL) {
        rtos_api_lib_set_rtos_log_enable(1);
    }

    return 0;
}

int rtos_log_deinit(void)
{
    rtos_api_lib_set_rtos_log_enable(0);
    rtos_api_lib_munmap(&stream.map);
    ambaipc_svc_unregister(AMBA_RPC_PROG_LU_RTOS_LOG_PROG_ID, AMBA_RPC_PROG_LU_RTOS_LOG_VER);

    return 0;
}

static void signal_handler(int signal)
{
    static int bExit = 0;
    if (bExit == 0) {
        bExit = 1;
        debug_line("rtos_log exit by signal:%d\n", signal);
        rtos_log_deinit();
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

    if (rtos_log_init() < 0) {
        rtos_log_deinit();
        return -1;
    }
    //wait
    sem_init(&main_sem, 0, 0);
    sem_wait(&main_sem);
    sem_destroy(&main_sem);

    return 0;
}


