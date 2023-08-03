#include "aipc_user.h"
#include "AmbaIPC_RpcProg_LU_LinuxTimeService.h"
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

static void ipc_get_cur_time(void *msg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    response_time_s response_time;
    struct timeval tv;
    int time_zone = *((int *)msg);

    gettimeofday(&tv, NULL);
    response_time.tv_sec = tv.tv_sec;
    response_time.tv_usec = tv.tv_usec;

    time_t local_time = (time_t)(tv.tv_sec + time_zone);
    struct tm *local_tm = gmtime(&local_time);
    response_time.local_time.year = local_tm->tm_year + 1900;
    response_time.local_time.month = local_tm->tm_mon + 1;
    response_time.local_time.day = local_tm->tm_mday;
    response_time.local_time.hour = local_tm->tm_hour;
    response_time.local_time.minute = local_tm->tm_min;
    response_time.local_time.second = local_tm->tm_sec;

    time_t utc_time = (time_t)(tv.tv_sec);
    struct tm *utc_tm = gmtime(&utc_time);
    response_time.utc_time.year = utc_tm->tm_year + 1900;
    response_time.utc_time.month = utc_tm->tm_mon + 1;
    response_time.utc_time.day = utc_tm->tm_mday;
    response_time.utc_time.hour = utc_tm->tm_hour;
    response_time.utc_time.minute = utc_tm->tm_min;
    response_time.utc_time.second = utc_tm->tm_sec;

    //send ipc reply
    memcpy(pRet->pResult, &response_time, sizeof(response_time));
    pRet->Length = sizeof(response_time_s);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

static void signal_handler(int signal)
{
    static int bExit = 0;
    if (bExit == 0) {
        bExit = 1;
        ambaipc_svc_unregister(AMBA_RPC_PROG_LU_LINUX_TIME_SERVICE_PROG_ID, AMBA_RPC_PROG_LU_LINUX_TIME_SERVICE_VER);
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
    AMBA_IPC_PROG_INFO_s prog_info;
    int ret = 0;

    capture_all_signal();

    if (daemon(0, 1)) {
        exit(1);
    }
    prog_info.ProcNum = 1;
    prog_info.pProcInfo = malloc(prog_info.ProcNum * sizeof(AMBA_IPC_PROC_s));
    prog_info.pProcInfo[0].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[0].Proc = (AMBA_IPC_PROC_f)&ipc_get_cur_time;
    ret = ambaipc_svc_register(AMBA_RPC_PROG_LU_LINUX_TIME_SERVICE_PROG_ID, AMBA_RPC_PROG_LU_LINUX_TIME_SERVICE_VER, "linux_time_service", &prog_info, 1);
    if (ret != 0) {
        free(prog_info.pProcInfo);
        return -1;
    }
    free(prog_info.pProcInfo);

    sem_init(&main_sem, 0, 0);
    sem_wait(&main_sem);
    sem_destroy(&main_sem);

    return 0;
}

