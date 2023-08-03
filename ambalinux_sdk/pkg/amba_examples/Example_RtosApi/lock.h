//lock.h
#ifndef __LOCK_H_
#define __LOCK_H_

#include <pthread.h>

typedef struct MUTEX_PACKAGE {
    pthread_mutex_t lock;
    pthread_mutexattr_t lock_attr;
    unsigned int flag;
    int id;
} mutex_info_t ;

extern mutex_info_t *mutex_info;

#endif//__LOCK_H_

