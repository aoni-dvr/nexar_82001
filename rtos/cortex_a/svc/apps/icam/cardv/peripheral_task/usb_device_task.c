#include "usb_device_task.h"

// msg task
static AMBA_KAL_TASK_t usb_device_task;
#define USB_DEVICE_TASK_STACK_SIZE (32 * 1024)
static unsigned char usb_device_task_stack[USB_DEVICE_TASK_STACK_SIZE] __attribute__((aligned(4))) __attribute__((section(".bss.noinit")));
static int running = 0;

static void *usb_device_task_entry(void *arg);

int usb_device_task_start(void)
{
    unsigned int ret = 0;

    running = 1;
    ret = AmbaKAL_TaskCreate(&usb_device_task,
                            "usb_device_task",
                            180,
                            (AMBA_KAL_TASK_ENTRY_f)usb_device_task_entry,
                            0,
                            usb_device_task_stack,
                            USB_DEVICE_TASK_STACK_SIZE,
                            AMBA_KAL_AUTO_START
                            );
    if (ret != KAL_ERR_NONE) {
        debug_line("usb_device_task create fail");
        running = 0;
        return -1;
    }

    return 0;
}

int usb_device_task_stop(void)
{
    running = 0;

    return 0;
}

static void *usb_device_task_entry(void *arg)
{
    int connected = 0;
    int last_connected = -1;

    while (running) {
        if (app_helper.check_usb_left_connected() || app_helper.check_usb_right_connected()) {
            connected = 1;
        } else {
            connected = 0;
        }
        if (last_connected >= 0 && last_connected != connected) {
            ipc_event_s event;
            memset(&event, 0, sizeof(event));
            event.event_id = NOTIFY_USB_LINE_STATE_CHANGE_ID;
            event.param = connected;
            linux_api_service_notify_event(event);
        }
        last_connected = connected;
        sleep(1);
    }

	return NULL;
}


