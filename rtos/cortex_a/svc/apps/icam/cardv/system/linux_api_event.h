#ifndef __LINUX_API_EVENT_H__
#define __LINUX_API_EVENT_H__

typedef enum _event_id_e_ {
    NOTIFY_POWER_STATE_CHANGE_ID = 0,
    NOTIFY_LED_IRQ_STATE_CHANGE_ID,
    NOTIFY_USB_LINE_STATE_CHANGE_ID,
    NOTIFY_BUTTON_STATE_LONG_PRESS,
    NOTIFY_BUTTON_STATE_SHORT_PRESS,
    NOTIFY_FILE_CREATED,
    NOTIFY_FILE_DELETED,
    NOTIFY_CAMERA_SHUTDOWN,
    NOTIFY_MAIN_MCU_UPDATE,
    NOTIFY_LTE_MCU_UPDATE,
    NOTIFY_ACC_CONNECTED,
    NOTIFY_ACC_DISCONNECTED,
} event_id_e;

typedef struct _file_arg_s_ {
    char path[64];
    unsigned int file_time;
    unsigned long long file_size;
} file_arg_s;

typedef union _ipc_arg_u_ {
    file_arg_s file_arg;
} ipc_arg_u;

typedef struct _ipc_event_s {
    int event_id;
    int param;
    int param2;
    ipc_arg_u arg;
} ipc_event_s;

int linux_api_service_notify_event(ipc_event_s event);

#endif//__LINUX_API_EVENT_H__

