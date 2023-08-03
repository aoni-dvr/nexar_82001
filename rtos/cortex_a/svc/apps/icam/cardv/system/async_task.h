#ifndef __ASYNC_QUEUE_H__
#define __ASYNC_QUEUE_H__

#include <app_base.h>

typedef struct _async_message_s_ {
    unsigned int msg_id;
    unsigned int param1;
    unsigned int param2;
    unsigned int param3;
} async_message_s;

typedef enum _async_msg_id_e_ {
    ASYNC_MSG_ID_FORAMT_SD = 1,
    ASYNC_MSG_ID_NUM
} async_msg_id_e;

int async_task_start(void);
int async_msg_queue_send(unsigned int msg_id, unsigned int param1, unsigned int param2, unsigned int param3);
int async_msg_queue_recv(async_message_s *msg);

#endif//__ASYNC_QUEUE_H__

