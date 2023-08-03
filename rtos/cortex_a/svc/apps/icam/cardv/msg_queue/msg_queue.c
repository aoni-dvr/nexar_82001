#include "msg_queue.h"
#include <app_base.h>

// msg queue
static AMBA_KAL_MSG_QUEUE_t msg_queue;
#define APP_MSG_QUEUE_SIZE (256)
static app_message_s app_msg_pool[APP_MSG_QUEUE_SIZE] __attribute__((section(".bss.noinit")));

// msg task
static AMBA_KAL_TASK_t msg_task;
#define APP_MSG_TASK_STACK_SIZE (1024 * 1024)
static unsigned char msg_task_stack[APP_MSG_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));

int app_msg_queue_init(void *(*entry)(void *))
{
    unsigned int ret = 0;

    // create msg queue
    ret = AmbaKAL_MsgQueueCreate(&msg_queue,
                                "app_msg_queue",
                                sizeof(app_message_s),
                                app_msg_pool,
                                APP_MSG_QUEUE_SIZE);
    if (ret != KAL_ERR_NONE) {
        debug_line("app_msg_queue create fail");
        return -1;
    }
    //create msg task
    ret = AmbaKAL_TaskCreate(&msg_task,
                            "app_msg_task",
                            SVC_HANDLER_TASK_PRI,
                            (AMBA_KAL_TASK_ENTRY_f)entry,
                            0,
                            msg_task_stack,
                            APP_MSG_TASK_STACK_SIZE,
                            0);
    if (ret != KAL_ERR_NONE) {
        debug_line("app_msg_task create fail");
        return -1;
    }
    AmbaKAL_TaskSetSmpAffinity(&msg_task, SVC_HANDLER_TASK_CPU_BITS);
    AmbaKAL_TaskResume(&msg_task);

    return 0;
}

int app_msg_queue_send(unsigned int msg_id, unsigned int param1, unsigned int param2, unsigned int param3)
{
    app_message_s msg;

    msg.msg_id = msg_id;
    msg.param1 = param1;
    msg.param2 = param2;
    msg.param3 = param3;
    if (AmbaKAL_MsgQueueSend(&msg_queue, &msg, AMBA_KAL_NO_WAIT) != KAL_ERR_NONE) {
        debug_line("<%s> send fail", __func__);
        return -1;
    }

    return 0;
}

int app_msg_queue_recv(app_message_s *msg)
{
    if (AmbaKAL_MsgQueueReceive(&msg_queue, msg, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {        
        debug_line("<%s> recv fail", __func__);
        return -1;
    }

    return 0;
}

int app_msg_queue_delete(void)
{
    return 0;
}

