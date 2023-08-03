#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "lock.h"

mutex_info_t *mutex_info = NULL;

const int create_sharemem(const unsigned int len) {
    int key = ftok("/tmp", 127);
    return shmget(key, len, IPC_CREAT | 0666);
}

const int init_mutex(void* pthis) {
    mutex_info_t * mp = (mutex_info_t *)pthis;
    pthread_mutexattr_init(&(mp->lock_attr));
    pthread_mutexattr_setpshared(&(mp->lock_attr), PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(mp->lock), &(mp->lock_attr));

    return 0;
}

__attribute__((constructor)) static void pre_init(void)
{
	int id = create_sharemem(sizeof(mutex_info_t));
	mutex_info = (mutex_info_t *)shmat(id, NULL, SHM_R | SHM_W);
	if (mutex_info->flag != 0x5a5a5a5a) {
        mutex_info->flag = 0x5a5a5a5a;
		mutex_info->id = id;
		init_mutex(mutex_info);
	}
}

__attribute__((destructor)) static void late_destory(void)
{
}

