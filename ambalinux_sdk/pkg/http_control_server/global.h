#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "rtos_api_lib.h"

typedef struct _gloabl_s_ {
    unsigned char authed;
    dev_info_s dev_info;
} global_s;

extern global_s global_data;

#endif//__GLOBAL_H__

