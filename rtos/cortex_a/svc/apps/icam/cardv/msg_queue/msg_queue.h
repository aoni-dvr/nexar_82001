#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__

typedef struct _app_message_s_ {
    unsigned int msg_id;
    unsigned int param1;
    unsigned int param2;
    unsigned int param3;
} app_message_s;

int app_msg_queue_init(void *(*entry)(void *));
int app_msg_queue_send(unsigned int msg_id, unsigned int param1, unsigned int param2, unsigned int param3);
int app_msg_queue_recv(app_message_s *msg);
int app_msg_queue_delete(void);

typedef enum _app_msg_id_e_ {
    APP_MSG_ID_TIMER_1HZ = 1,
    APP_MSG_ID_TIMER_2HZ,
    APP_MSG_ID_TIMER_30S,
    APP_MSG_ID_CAM_LIVEVIEW,
    APP_MSG_ID_CAM_START_EVENT_RECORD,
    APP_MSG_ID_CAM_STOP_EVENT_RECORD,
    APP_MSG_ID_CAM_CAPTURE_PIV_MJPG,
    APP_MSG_ID_CAM_CAPTURE_PIV_RAW,
    APP_MSG_ID_CAM_CAPTURE_PIV_DONE,
    APP_MSG_ID_CAM_CAPTURE_THM,
    APP_MSG_ID_CAM_CAPTURE_THM_DONE,
    APP_MSG_ID_SD_INSERT,
    APP_MSG_ID_SD_REMOVE,
    APP_MSG_ID_SD_IDLE,
    APP_MSG_ID_SD_ERROR,
    APP_MSG_ID_USB_INSERT,
    APP_MSG_ID_USB_REMOVE,
    APP_MSG_ID_LINUX_BOOT_DONE,
    APP_MSG_ID_WIFI_BOOT_DONE,
    APP_MSG_ID_BT_BOOT_DONE,
    APP_MSG_ID_USB_WIFI_BOOT_DONE,
    APP_MSG_ID_LTE_ON,
    APP_MSG_ID_LTE_BOOT_DONE,
    APP_MSG_ID_POWER_BUTTON,
    APP_MSG_ID_FORMAT_SD,
    APP_MSG_ID_FORMAT_SD_DONE,
    APP_MSG_ID_FORMAT_EMMC_DONE,
    APP_MSG_ID_DEFAULT_SETTING,
    APP_MSG_ID_REBOOT,
    APP_MSG_ID_BUTTON_EVENT,
    APP_MSG_ID_FACTORY_RESET,
    APP_MSG_ID_LOW_BATTERY,
    APP_MSG_ID_ACC_CONNECTED,
    APP_MSG_ID_ACC_DISCONNECTED,
    APP_MSG_ID_NUM
} app_msg_id_e;

#endif//__MSG_QUEUE_H__

