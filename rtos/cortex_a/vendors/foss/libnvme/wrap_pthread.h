#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif

typedef AMBA_KAL_MUTEX_t pthread_mutex_t;

#define pthread_mutex_init(mutex,attr)	AmbaKAL_MutexCreate(mutex,NULL)
#define pthread_mutex_lock(mutex)	AmbaKAL_MutexTake(mutex,AMBA_KAL_WAIT_FOREVER)
#define pthread_mutex_unlock	AmbaKAL_MutexGive
#define pthread_mutex_destroy	AmbaKAL_MutexDelete

#define nvme_usleep(a) AmbaKAL_TaskSleep(a/1000)
#define nvme_msleep(a) AmbaKAL_TaskSleep(a)
#define sleep(a)       AmbaKAL_TaskSleep(a*1000)
