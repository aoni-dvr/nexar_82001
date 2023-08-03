/**
 *  @file main.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include <stdarg.h>
//#define AMBAMMU_TEST
#if defined(AMBAMMU_TEST)
#include <fcntl.h>
#include "AmbaMMU.h"
#endif

AMBA_KAL_TASK_t Task1, Task2, Task3;
AMBA_KAL_SEMAPHORE_t Semaphore;
AMBA_KAL_MUTEX_t Mutex;
AMBA_KAL_MSG_QUEUE_t MsgQueue;
AMBA_KAL_EVENT_FLAG_t EventFlag;

#define MQ_MSGSIZE 4
#define MQ_MAXMSG 2
char AMBA_MSG[MQ_MSGSIZE] = "abc";
int NG = 0;
#define AMBAKAL_SEMAPHORE_TEST
#define AMBAKAL_MUTEX_TEST
#define AMBAKAL_MSGQUEUE_TEST
#define AMBAKAL_EVENTFLAG_TEST

void debug(const char *fmt, ...)
{
    struct timespec ts;
    char Buf[512];
    va_list args;
    INT32 n;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    sprintf(Buf, "%ld.%ld ", ts.tv_sec, ts.tv_nsec);

    //fprintf(stderr, "%s %d", Buf, strlen(Buf));

    va_start(args, fmt);
    n = vsnprintf((char *)Buf + strlen(Buf), 512-strlen(Buf), (const char *)fmt, args);
    Buf[n + strlen(Buf)] = '\0';
    va_end(args);

    fprintf(stderr, "%s", Buf);
}

void *thread1 (void *EntryArg)
{
    char msg[MQ_MSGSIZE];
    UINT32 flag;
    AMBA_KAL_SEMAPHORE_INFO_s SemaphoreInfo;
    AMBA_KAL_TASK_t Task = 0;
    AMBA_KAL_TASK_t *pTask = NULL;
    struct timespec ts1, ts2;

    if ((*(int *)EntryArg) != 1) {
        debug("\033""[1;31m" "%s(%d) EntryArg failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }

    debug("[%s] AmbaKAL_TaskIdentify NG case\n", __func__);
    if (0 == AmbaKAL_TaskIdentify(&pTask)) {
        debug("\033""[1;31m" "%s(%d) NG test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }

    debug("[%s] AmbaKAL_TaskIdentify OK case\n", __func__);
    pTask = &Task;
    AmbaKAL_TaskIdentify(&pTask);
    if (0 == pthread_equal(*pTask, Task1)) {
        debug("[%s] Task1 %d not equal to %d\n", __func__, Task1, Task);
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
#if defined(AMBAKAL_SEMAPHORE_TEST)
    debug("[%s] AmbaKAL_SemaphoreTake OK case\n", __func__);
    if (0!= AmbaKAL_SemaphoreTake(&Semaphore, 1000)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }

    if (0!= AmbaKAL_SemaphoreQuery(&Semaphore, &SemaphoreInfo)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
    if (SemaphoreInfo.CurrentCount != 0) {
        debug("\033""[1;31m" "%s(%d) SemaphoreInfo.CurrentCount %d != 1\n" "\033""[0m", __func__, __LINE__, SemaphoreInfo.CurrentCount);
        NG = 1;
    }

#endif
#if defined(AMBAKAL_MUTEX_TEST)
    debug("[%s] AmbaKAL_MutexTake OK case\n", __func__);
    if (0!= AmbaKAL_MutexTake(&Mutex, 1000)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
#endif
#if defined(AMBAKAL_MSGQUEUE_TEST)
    debug("[%s] AmbaKAL_MsgQueueReceive OK case\n", __func__);
    if (0!= AmbaKAL_MsgQueueReceive(&MsgQueue, msg, 1000)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
#endif

#if defined(AMBAKAL_EVENTFLAG_TEST)
    debug("\033""[1;32m" "[%s] AmbaKAL_EventFlagGet no wait NG case\n" "\033""[0m", __func__);
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    if (0 == AmbaKAL_EventFlagGet(&EventFlag, 0x1, KAL_FLAGS_WAIT_ALL, 0, &flag, AMBA_KAL_NO_WAIT)) {
        debug("\033""[1;31m" "%s(%d) NG test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        if (ts2.tv_sec > ts1.tv_sec + 1) {
            debug("\033""[1;31m" "%s(%d) NG test-case failure ts1 %ld ts2 %ld\n" "\033""[0m", __func__, __LINE__, ts1.tv_sec, ts2.tv_sec);
            NG = 1;
        }
    }

    debug("\033""[1;32m" "[%s] AmbaKAL_EventFlagGet timeout NG case\n" "\033""[0m", __func__);
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    if (0 == AmbaKAL_EventFlagGet(&EventFlag, 0x1, KAL_FLAGS_WAIT_ALL, 1, &flag, 1000)) {
        debug("\033""[1;31m" "%s(%d) NG test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        if (ts2.tv_sec > ts1.tv_sec + 1) {
            debug("\033""[1;31m" "%s(%d) NG test-case failure ts1 %ld ts2 %ld\n" "\033""[0m", __func__, __LINE__, ts1.tv_sec, ts2.tv_sec);
            NG = 1;
        }
    }

    AmbaKAL_TaskSleep(1000);
    debug("\033""[1;32m" "[%s] AmbaKAL_EventFlagSet 0x1 OK case\n" "\033""[0m", __func__);
    if (0 != AmbaKAL_EventFlagSet(&EventFlag, 0x1)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
#endif
#if defined(AMBAKAL_MUTEX_TEST)
    AmbaKAL_TaskSleep(3*1000);
    debug("[%s] AmbaKAL_MutexGive OK case\n", __func__);
    if (0!= AmbaKAL_MutexGive(&Mutex)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
#endif
    debug("[%s] leave\n", __func__);
    AmbaKAL_TaskSleep(100*1000);
    return NULL;
}

void *thread2 (void *EntryArg)
{
    char msg[MQ_MSGSIZE];
    UINT32 flag;
    AMBA_KAL_SEMAPHORE_INFO_s SemaphoreInfo;
    struct timespec ts1, ts2;
    AMBA_KAL_TASK_t Task = 0;
    AMBA_KAL_TASK_t *pTask = NULL;

    if ((*(int *)EntryArg) != 2) {
        debug("\033""[1;31m" "%s(%d) EntryArg failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }

    debug("[%s] AmbaKAL_TaskIdentify NG case\n", __func__);
    if (0 == AmbaKAL_TaskIdentify(&pTask)) {
        debug("\033""[1;31m" "%s(%d) NG test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }

    debug("[%s] AmbaKAL_TaskIdentify OK case\n", __func__);
    pTask = &Task;
    AmbaKAL_TaskIdentify(&pTask);
    if (0 == pthread_equal(*pTask, Task2)) {
        debug("[%s] Task2 %d not equal to %d\n", __func__, Task2, Task);
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }

#if defined(AMBAKAL_SEMAPHORE_TEST)
    debug("[%s] AmbaKAL_SemaphoreTake NG case\n", __func__);
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    if (0 == AmbaKAL_SemaphoreTake(&Semaphore, 1000)) {
        debug("\033""[1;31m" "%s(%d) NG test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        if (ts2.tv_sec != ts1.tv_sec + 1) {
            debug("\033""[1;31m" "%s(%d) NG test-case failure ts1 %ld ts2 %ld\n" "\033""[0m", __func__, __LINE__, ts1.tv_sec, ts2.tv_sec);
            NG = 1;
        }
    }
    if (0!= AmbaKAL_SemaphoreQuery(&Semaphore, &SemaphoreInfo)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
    if (SemaphoreInfo.CurrentCount != 0) {
        debug("\033""[1;31m" "%s(%d) SemaphoreInfo.CurrentCount %d != 1\n" "\033""[0m", __func__, __LINE__, SemaphoreInfo.CurrentCount);
        NG = 1;
    }
#endif
#if defined(AMBAKAL_MUTEX_TEST)
    debug("[%s] AmbaKAL_MutexTake NG case\n", __func__);
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    if (0 == AmbaKAL_MutexTake(&Mutex, 1000)) {
        debug("\033""[1;31m" "%s(%d) NG test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        if (ts2.tv_sec != ts1.tv_sec + 1) {
            debug("\033""[1;31m" "%s(%d) NG test-case failure ts1 %ld ts2 %ld\n" "\033""[0m", __func__, __LINE__, ts1.tv_sec, ts2.tv_sec);
            NG = 1;
        }
    }
#endif
#if defined(AMBAKAL_MSGQUEUE_TEST)
    debug("[%s] AmbaKAL_MsgQueueReceive OK case\n", __func__);
    if (0!= AmbaKAL_MsgQueueReceive(&MsgQueue, msg, 1000)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
#endif
#if defined(AMBAKAL_EVENTFLAG_TEST)
    debug("\033""[1;32m" "[%s] AmbaKAL_EventFlagGet OK case\n" "\033""[0m", __func__);
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    if (0 != AmbaKAL_EventFlagGet(&EventFlag, 0x1, KAL_FLAGS_WAIT_ANY, 0, &flag, 1000)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m",  __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    } else {
        debug("\033""[1;32m" "%s(%d) got flag 0x%x  \n" "\033""[0m", __func__, __LINE__, flag);
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        if ((0 == (flag & 0x1))||
            (ts2.tv_sec > ts1.tv_sec + 1)) {
            debug("\033""[1;31m" "%s(%d) err flag 0x%x ts1 %ld ts2 %ld\n" "\033""[0m", __func__, __LINE__, flag, ts1.tv_sec, ts2.tv_sec);
            NG = 1;
        }
    }

    AmbaKAL_TaskSleep(1000);
    debug("\033""[1;32m" "[%s] AmbaKAL_EventFlagSet 0x3 OK case\n" "\033""[0m", __func__);
    if (0 != AmbaKAL_EventFlagSet(&EventFlag, 0x3)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
#endif
    debug("[%s] leave\n", __func__);
    AmbaKAL_TaskSleep(100*1000);
    return NULL;
}

void *thread3 (void *EntryArg)
{
    char msg[MQ_MSGSIZE];
    UINT32 flag;
    AMBA_KAL_SEMAPHORE_INFO_s SemaphoreInfo;
    AMBA_KAL_TASK_t Task = 0;
    AMBA_KAL_TASK_t *pTask = NULL;
    struct timespec ts1, ts2;
    AMBA_KAL_EVENT_FLAG_INFO_s flag_info;

    if ((*(int *)EntryArg) != 3) {
        debug("\033""[1;31m" "%s(%d) EntryArg failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }

    /*AutoStart=1 starts too fast, wait for value of Task3*/
    AmbaKAL_TaskSleep(100);
    debug("[%s] AmbaKAL_TaskIdentify NG case\n", __func__);
    if (0 == AmbaKAL_TaskIdentify(&pTask)) {
        debug("\033""[1;31m" "%s(%d) NG test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }

    debug("[%s] AmbaKAL_TaskIdentify OK case\n", __func__);
    pTask = &Task;
    AmbaKAL_TaskIdentify(&pTask);
    if (0 == pthread_equal(*pTask, Task3)) {
        debug("[%s] Task3 %d not equal to %d\n", __func__, Task3, Task);
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }

    if (NG == 1) {
        exit(1);
    }
    debug("AmbaKAL_Task PASS\n\n");

#if defined(AMBAKAL_SEMAPHORE_TEST)
    debug("[%s] AmbaKAL_SemaphoreTake NG case\n", __func__);
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    if (0 == AmbaKAL_SemaphoreTake(&Semaphore, 1000)) {
        debug("\033""[1;31m" "%s(%d) NG test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        if (ts2.tv_sec != ts1.tv_sec + 1) {
            debug("\033""[1;31m" "%s(%d) NG test-case failure ts1 %ld ts2 %ld\n" "\033""[0m", __func__, __LINE__, ts1.tv_sec, ts2.tv_sec);
            NG = 1;
        }
    }

    if (0!= AmbaKAL_SemaphoreQuery(&Semaphore, &SemaphoreInfo)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
    if (SemaphoreInfo.CurrentCount != 0) {
        debug("\033""[1;31m" "%s(%d) SemaphoreInfo.CurrentCount %d != 1\n" "\033""[0m", __func__, __LINE__, SemaphoreInfo.CurrentCount);
        NG = 1;
    }
    if (NG == 1) {
        exit(1);
    }
    debug("AmbaKAL_Semaphore PASS\n\n");
#endif
#if defined(AMBAKAL_MUTEX_TEST)
    debug("[%s] AmbaKAL_MutexTake NG case\n", __func__);
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    if (0 == AmbaKAL_MutexTake(&Mutex, 1000)) {
        debug("\033""[1;31m" "%s(%d) NG test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        if (ts2.tv_sec != ts1.tv_sec + 1) {
            debug("\033""[1;31m" "%s(%d) NG test-case failure ts1 %ld ts2 %ld\n" "\033""[0m", __func__, __LINE__, ts1.tv_sec, ts2.tv_sec);
            NG = 1;
        }
    }
    if (NG == 1) {
        exit(1);
    }
    debug("AmbaKAL_Mutex PASS\n\n");
#endif
#if defined(AMBAKAL_MSGQUEUE_TEST)
    debug("[%s] AmbaKAL_MsgQueueReceive NG case\n", __func__);
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    if (0 == AmbaKAL_MsgQueueReceive(&MsgQueue, msg, 1000)) {
        debug("\033""[1;31m" "%s(%d) NG test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        if (ts2.tv_sec != ts1.tv_sec + 1) {
            debug("\033""[1;31m" "%s(%d) NG test-case failure ts1 %ld ts2 %ld\n" "\033""[0m", __func__, __LINE__, ts1.tv_sec, ts2.tv_sec);
            NG = 1;
        }
    }
    if (NG == 1) {
        exit(1);
    }
    debug("AmbaKAL_MsgQueue PASS\n\n");
#endif
#if defined(AMBAKAL_EVENTFLAG_TEST)
    debug("\033""[1;32m" "[%s] AmbaKAL_EventFlagGet clear OK case\n" "\033""[0m", __func__);
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    if (0 != AmbaKAL_EventFlagGet(&EventFlag, 0x3, KAL_FLAGS_WAIT_ALL, 1, &flag, KAL_WAIT_FOREVER)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m",  __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        debug("\033""[1;32m" "%s(%d) got flag 0x%x  \n" "\033""[0m", __func__, __LINE__, flag);
        if ((0x3 != (flag & 0x3))||
            (ts2.tv_sec > ts1.tv_sec + 1)) {
            debug("\033""[1;31m" "%s(%d) err flag 0x%x ts1 %ld ts2 %ld\n" "\033""[0m", __func__, __LINE__, flag, ts1.tv_sec, ts2.tv_sec);
            NG = 1;
        }
    }

    debug("\033""[1;32m" "[%s] AmbaKAL_EventFlagGet NG case\n" "\033""[0m", __func__);
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    if (0 == AmbaKAL_EventFlagGet(&EventFlag, 0x1, KAL_FLAGS_WAIT_ANY, 0, &flag, 1000)) {
        debug("\033""[1;31m" "%s(%d) NG test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        if (ts2.tv_sec != ts1.tv_sec + 1) {
            debug("\033""[1;31m" "%s(%d) NG test-case failure ts1 %ld ts2 %ld\n" "\033""[0m", __func__, __LINE__, ts1.tv_sec, ts2.tv_sec);
            NG = 1;
        }
    }

    debug("\033""[1;32m" "[%s] AmbaKAL_EventFlagSet 0x3 OK case\n" "\033""[0m", __func__);
    if (0 != AmbaKAL_EventFlagSet(&EventFlag, 0x3)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }

    debug("\033""[1;32m" "[%s] AmbaKAL_EventFlagQuery OK case\n" "\033""[0m", __func__);
    if (0 != AmbaKAL_EventFlagQuery(&EventFlag, &flag_info)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
    if ((flag_info.CurrentFlags != EventFlag.flag) ||
        (flag_info.CurrentFlags != 0x3)) {
        debug("\033""[1;31m" "%s(%d) err %d != %d != 0x3\n" "\033""[0m", __func__, __LINE__, flag_info.CurrentFlags, EventFlag.flag);
        NG = 1;
    }

    debug("\033""[1;32m" "[%s] AmbaKAL_EventFlagClear 0x1 OK case\n" "\033""[0m", __func__);
    if (0!= AmbaKAL_EventFlagClear(&EventFlag, 0x1)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }

    debug("\033""[1;32m" "[%s] AmbaKAL_EventFlagQuery OK case\n" "\033""[0m", __func__);
    if (0 != AmbaKAL_EventFlagQuery(&EventFlag, &flag_info)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
    if ((flag_info.CurrentFlags != EventFlag.flag) ||
        (flag_info.CurrentFlags != 0x2)) {
        debug("\033""[1;31m" "%s(%d) err %d != %d != 0x2\n" "\033""[0m", __func__, __LINE__, flag_info.CurrentFlags, EventFlag.flag);
        NG = 1;
    }
    if (NG == 1) {
        exit(1);
    }
    debug("AmbaKAL_EventFlag PASS\n\n");
#endif

    debug("[%s] leave\n", __func__);
    AmbaKAL_TaskSleep(100*1000);
    return NULL;
}


#define MAX_MSG_SIZE 4096
#define MAX_MSGS     2

int main(int argc, char *argv[])
{
    struct _clockperiod new, old;
#define TestStackSize 16384U
    static UINT8 Stack[2][TestStackSize] __attribute__((section(".bss.noinit")));
    static char taskname[2][32] = { "1Task", "Task2" };
    int i;
    AMBA_KAL_MSG_QUEUE_INFO_s MsgQueueInfo;
    struct timespec ts1, ts2;
    AMBA_KAL_SEMAPHORE_INFO_s SemaphoreInfo;
    static int A1 = 1, A2 = 2, A3 = 3;

#if defined(AMBAMMU_TEST)
#define MAP_SIZE 8192
    int mmu_fd;
    void *virt;
    ULONG phys;
    INT64 map_diff;

    mmu_fd = posix_typed_mem_open("below4G", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    if (mmu_fd == -1) {
        debug("open below4G return %d: %s; now try /ram/cv_schdr\n", errno, strerror(errno));
        mmu_fd = posix_typed_mem_open("/ram/cv_schdr", O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
        if (mmu_fd == -1) {
            debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
            return -1;
        }
    }

    virt = mmap (NULL, MAP_SIZE, PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_SHARED, mmu_fd, 0);
    if (virt == MAP_FAILED) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        return -1;
    }

    memset(virt, 0x00, MAP_SIZE);

    if (0!= AmbaMMU_VirtToPhys(virt, &phys)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
    if ((UINT64)virt > phys) {
        map_diff = (UINT64)virt - phys;
        debug("phys 0x%lx + 0x%lx = virt 0x%lx\n", phys, map_diff, (UINT64)virt);
    } else {
        map_diff = phys - (UINT64)virt;
        debug("virt 0x%lx + 0x%lx = phys 0x%lx\n", (UINT64)virt, map_diff, phys);
    }

    *((volatile UINT32 *) (virt)) = 0x12345678;
    *((volatile UINT32 *) (virt + 4)) = 0xdeadbeef;
    *((volatile UINT32 *) (virt + 8)) = 0xcafeface;

    *((volatile UINT32 *) (virt + MAP_SIZE - 12)) = 0x12345678;
    *((volatile UINT32 *) (virt + MAP_SIZE - 8)) = 0xdeadbeef;
    *((volatile UINT32 *) (virt + MAP_SIZE - 4)) = 0xcafeface;

    close(mmu_fd);

    virt = mmap_device_memory((void *) phys, MAP_SIZE, PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_FIXED, phys);
    if ((virt == MAP_FAILED) || (virt != (void *) phys)) {
        debug("\033""[1;31m" "%s(%d) MMU test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }

    debug("check physical memory 0x%lx\n", phys);
    if (*((volatile UINT32 *) (virt)) != 0x12345678) {
        debug("\033""[1;31m" "%s(%d) MMU test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }
    if (*((volatile UINT32 *) (virt + 4)) != 0xdeadbeef) {
        debug("\033""[1;31m" "%s(%d) MMU test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }
    if (*((volatile UINT32 *) (virt + 8)) != 0xcafeface) {
        debug("\033""[1;31m" "%s(%d) MMU test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }

    debug("check physical memory 0x%lx\n", phys + MAP_SIZE - 12);
    if (*((volatile UINT32 *) (virt + MAP_SIZE - 12)) != 0x12345678) {
        debug("\033""[1;31m" "%s(%d) MMU test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }
    if (*((volatile UINT32 *) (virt + MAP_SIZE - 8)) != 0xdeadbeef) {
        debug("\033""[1;31m" "%s(%d) MMU test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }
    if (*((volatile UINT32 *) (virt + MAP_SIZE - 4)) != 0xcafeface) {
        debug("\033""[1;31m" "%s(%d) MMU test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    }

    munmap(virt, MAP_SIZE);

    return 0;
#endif

    (void)argc;
    (void)argv;

    ClockPeriod(CLOCK_REALTIME, NULL, &old, 0);
    if (old.nsec != 1000*1000) {
        debug("\033""[1;31m" "old clock resolution: %d ms\n" "\033""[0m", old.nsec/1000*1000);
        new.nsec=1000*1000;
        ClockPeriod(CLOCK_REALTIME, &new, NULL, 0);
        debug("\033""[1;31m" "new clock resolution: %d ms\n" "\033""[0m", new.nsec/1000*1000);
    }

    /* AmbaKAL_Semaphore */
    if (0!= AmbaKAL_SemaphoreCreate(&Semaphore, NULL, 0)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
    if (0!= AmbaKAL_SemaphoreGive(&Semaphore)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }

    if (0!= AmbaKAL_SemaphoreQuery(&Semaphore, &SemaphoreInfo)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }
    if (SemaphoreInfo.CurrentCount != 1) {
        debug("\033""[1;31m" "%s(%d) SemaphoreInfo.CurrentCount %d != 1\n" "\033""[0m", __func__, __LINE__, SemaphoreInfo.CurrentCount);
        NG = 1;
    }

    /* AmbaKAL_Mutex */
    if (0!= AmbaKAL_MutexCreate(&Mutex, NULL)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        NG = 1;
    }

    /* AmbaKAL_MsgQueue */
    if (0!= AmbaKAL_MsgQueueCreate(&MsgQueue, "kal", MQ_MSGSIZE, NULL, MQ_MSGSIZE*MQ_MAXMSG)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
        debug("did you run mqueue ? Check pidin\n");
        NG = 1;
    }
    /* AmbaKAL_MsgQueueFlush */
    if (0!= AmbaKAL_MsgQueueFlush(&MsgQueue)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }

    /* AmbaKAL_MsgQueueQuery*/
    if (0!= AmbaKAL_MsgQueueQuery(&MsgQueue, &MsgQueueInfo)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    if (MsgQueueInfo.NumEnqueued != 0) {
        debug("\033""[1;31m" "%s(%d) NumEnqueued %d NumAvailable %d\n" "\033""[0m", __func__, __LINE__, MsgQueueInfo.NumEnqueued, MsgQueueInfo.NumAvailable);
        NG = 1;
    }
    if (MsgQueueInfo.NumAvailable != 2) {
        debug("\033""[1;31m" "%s(%d) NumEnqueued %d NumAvailable %d\n" "\033""[0m", __func__, __LINE__, MsgQueueInfo.NumEnqueued, MsgQueueInfo.NumAvailable);
        NG = 1;
    }

    /* AmbaKAL_MsgQueueSend OK case */
    for (i = 1; i <= MQ_MAXMSG; i++) {
        if (0!= AmbaKAL_MsgQueueSend(&MsgQueue, AMBA_MSG, 1000)) {
            debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
            NG = 1;
        }
    }
    /* AmbaKAL_MsgQueueSend NG case */
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    if (0 == AmbaKAL_MsgQueueSend(&MsgQueue, AMBA_MSG, 1000)) {
        debug("\033""[1;31m" "%s(%d) NG test-case failure\n" "\033""[0m", __func__, __LINE__);
        NG = 1;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        if (ts2.tv_sec != ts1.tv_sec + 1) {
            debug("\033""[1;31m" "%s(%d) NG test-case failure ts1 %ld ts2 %ld\n" "\033""[0m", __func__, __LINE__, ts1.tv_sec, ts2.tv_sec);
            NG = 1;
        }
    }

    /* AmbaKAL_EventFlag */
    if (0!= AmbaKAL_EventFlagCreate(&EventFlag, NULL)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }

    /* AmbaKAL_Task */
    memset(&Stack[0][0], 0, TestStackSize);
    if (0!= AmbaKAL_TaskCreate(&Task1, taskname[0], 0U, thread1, &A1, &Stack[0][0], TestStackSize, 1)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    memset(&Stack[1][0], 0, TestStackSize);
    if (0!= AmbaKAL_TaskCreate(&Task2, taskname[1], 0U, thread2, &A2, &Stack[1][0], TestStackSize, 0)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    if (0!= AmbaKAL_TaskCreate(&Task3, NULL, 0, thread3, &A3, NULL, 0, 1)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    if (0!= AmbaKAL_TaskResume(&Task2)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }

    debug("[%s] sleep 10\n", __func__);
    AmbaKAL_TaskSleep(10*1000);
    debug("[%s] Terminating\n", __func__);

    /* Terminate */
    if (0 != AmbaKAL_TaskTerminate(&Task1)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    if (0 != AmbaKAL_TaskTerminate(&Task2)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    if (0 != AmbaKAL_TaskTerminate(&Task3)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    if (0 != AmbaKAL_TaskDelete(&Task1)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    if (0 != AmbaKAL_TaskDelete(&Task2)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    if (0 != AmbaKAL_TaskDelete(&Task3)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    if (0 != AmbaKAL_SemaphoreDelete(&Semaphore)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    if (0 != AmbaKAL_MutexDelete(&Mutex)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    if (0 != AmbaKAL_MsgQueueDelete(&MsgQueue)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }
    if (0 != AmbaKAL_EventFlagDelete(&EventFlag)) {
        debug("\033""[1;31m" "%s(%d) err %d: %s\n" "\033""[0m", __func__, __LINE__, errno, strerror(errno));
    }

    if (old.nsec != 1000*1000) {
        new.nsec=old.nsec;
        ClockPeriod(CLOCK_REALTIME, &new, NULL, 0);
        debug("\033""[1;31m" "restore clock resolution to %d ms\n" "\033""[0m", new.nsec/1000*1000);
    }

    return 0;
}
